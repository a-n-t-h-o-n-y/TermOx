/** @file
 * Shared implementation of the public line and multiline editors.
 */
#pragma once
#include <algorithm>
#include <functional>
#include <ox/clipboard.hpp>
#include <ox/label.hpp>
#include <ox/put.hpp>
#include <ox/scroll.hpp>
#include <ox/text_view.hpp>
#include <ox/widget.hpp>
#include <string>
#include <tuple>
namespace ox::detail {
/** Shared text-widget implementation exposed through LineEdit and TextBox.
 * Owns normalized UTF-8 editing state. Text-changing commands request layout/paint,
 * reveal the caret, and emit on_change only for a document revision. Selection changes
 * emit anchor and caret byte offsets, in that order. All callbacks and clipboard work run on the application thread.
 * Public presentation changes need refresh(); read_only controls user edits, not programmatic set_text().
 */
template <bool Multiline>
class TextControl : public Widget {
   public:
    using Widget::handle;
    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Initial UTF-8 document; line mode normalizes away tabs and line breaks. */
        std::string text = {};
        /** Display-row wrapping policy; LineEdit requires TextWrap::None. */
        TextWrap wrap = Multiline ? TextWrap::Word : TextWrap::None;
        /** Text alignment along its display direction. */
        Align align = Align::Left;
        /** Base drawing style; explicit colors take precedence over theme defaults. */
        Style style = {};
        /** Style patch applied to selected text. */
        StylePatch selection_style = {.background = {},
                                      .foreground = {},
                                      .enabled = Trait::Inverse,
                                      .disabled = {}};
        /** Text displayed when the current editable or selected value is empty. */
        std::string placeholder = {};
        /** Style patch for placeholder text, without modifying the stored document. */
        StylePatch placeholder_style = {.background = {},
                                        .foreground = colors::bright_black,
                                        .enabled = {},
                                        .disabled = {}};
        /** Style patch applied while the control has focus. */
        StylePatch focused_style = {};
        /** Style patch for the control when it does not have focus. */
        StylePatch blurred_style = {};
        /** Selected-text patch when the editor is not focused. */
        StylePatch inactive_selection_style = {};
        /** Enable the TextBox logical-line gutter; wrapped continuations do not repeat
         * numbers.
         */
        bool line_numbers = false;
        /** Highlight the logical line containing the caret. */
        bool highlight_line = false;
        /** Style patch for the line-number gutter. */
        StylePatch gutter_style = {};
        /** Style patch for the caret's logical line, before selection and gutter styling. */
        StylePatch current_line_style = {};
        /** Suppress user document edits while allowing navigation, selection, and copying. */
        bool read_only = false;
        /** Optional per-scalar insertion predicate; rejected input scalars are omitted. */
        std::function<bool(char32_t)> accept_character = {};
        /** Optional whole-document validator used on submit; intermediate editing
         * remains allowed.
         */
        std::function<bool(std::string_view)> validator = {};
        /** Keyboard- and pointer-focus eligibility for this control. */
        FocusPolicy focus_policy = FocusPolicy::Strong;
        /** Horizontal size bounds and growth weight. */
        SizePolicy width_policy = SizePolicy::flex();
        /** Vertical size bounds and growth weight. */
        SizePolicy height_policy =
            Multiline ? SizePolicy::flex() : SizePolicy::fixed(1);
    };
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style;
    /** Display-row wrapping policy; LineEdit requires TextWrap::None. */
    TextWrap wrap;
    /** Text alignment along its display direction. */
    Align align;
    /** Style patch applied to selected text. */
    StylePatch selection_style;
    /** Text displayed when the current editable or selected value is empty. */
    std::string placeholder;
    /** Style patch for placeholder text, without modifying the stored document. */
    StylePatch placeholder_style;
    /** Style patch applied while the control has focus. */
    StylePatch focused_style;
    /** Style patch for the control when it does not have focus. */
    StylePatch blurred_style;
    /** Selected-text patch when the editor is not focused. */
    StylePatch inactive_selection_style;
    /** Enable the TextBox logical-line gutter; wrapped continuations do not repeat numbers. */
    bool line_numbers;
    /** Highlight the logical line containing the caret. */
    bool highlight_line;
    /** Style patch for the line-number gutter. */
    StylePatch gutter_style;
    /** Style patch for the caret's logical line, before selection and gutter styling. */
    StylePatch current_line_style;
    /** Suppress user document edits while allowing navigation, selection, and copying. */
    bool read_only;
    /** Optional per-scalar insertion predicate; rejected input scalars are omitted. */
    std::function<bool(char32_t)> accept_character;
    /** Validate complete values on submit; intermediate editing remains allowed. */
    std::function<bool(std::string_view)> validator;
    /** Emits the normalized document after a text revision; arguments are borrowed
     * during emission.
     */
    sl::Signal<void(std::string const&)> on_change;
    /** Emits UTF-8 byte offsets (anchor, caret) when either changes; reversed selections
     * have caret < anchor.
     */
    sl::Signal<void(std::size_t anchor, std::size_t caret)> on_selection_change;
    /** Emits the complete document when submit() passes the optional whole-value validator. */
    sl::Signal<void(std::string const&)> on_submit;
    /** Emits the complete document when the optional whole-value validator rejects
     * submission.
     */
    sl::Signal<void(std::string const&)> on_invalid;
    /** Own and normalize initial text with the selected single/multiline mode and
     * appearance; construction emits no edit notifications.
     */
    explicit TextControl(Options x);
    /** Borrow the normalized UTF-8 document until a text mutation. */
    auto text() const -> std::string const& { return editor_.text(); }
    /** Borrow read-only editing state; use edit() for staged programmatic batches. */
    auto editor() const -> TextEdit const& { return editor_; }
    /** Replace normalized text and reset scrolling/history; return whether the document
     * changed, notifying changed text/selection.
     */
    auto set_text(std::string_view text) -> bool;
    /** Apply action to a copy, then commit its text and selection together.
     * Return whether text or selection changed. An exception from action leaves
     * the staged edits unapplied. Destroying the owner in action returns false.
     * @throws std::invalid_argument Empty action or changed single/multiline mode.
     * @throws std::logic_error Action also changed the live owner's text or selection.
     */
    auto edit(std::function<void(TextEdit&)> action) -> bool;
    /** Move to a valid UTF-8 grapheme boundary, reveal the caret, and notify changed
     * selection; invalid offsets throw.
     */
    auto set_caret(std::size_t offset, bool extend = false) -> bool;
    /** Select all text, request repaint, and notify if selection changed. */
    auto select_all() -> bool;
    /** Normalize/filter insertion, replace the selection, and retain undo history;
     * return whether the document changed.
     */
    auto insert(std::string_view text) -> bool;
    /** Stream complete lines without copying the editor; preserve selection. */
    auto append_lines(std::string_view text, std::size_t discard_prefix = 0) -> bool
        requires Multiline;
    /** Undo a retained edit, reveal its restored caret, and notify changed
     * text/selection; return false without history.
     */
    auto undo() -> bool;
    /** Replay a retained undone edit and notify changed text/selection; return false
     * without redo history.
     */
    auto redo() -> bool;
    /** Validate the whole value and emit on_submit on success or on_invalid on rejection.
     * Return false if disabled, rejected, or if the validator destroys the owner
     * or changes its text. Return true after emitting on_submit.
     */
    auto submit() -> bool;
    /** Write selected text to the application clipboard; return false for an empty selection.
     * @throws std::logic_error A nonempty selection has no attached clipboard.
     * @throws std::invalid_argument The clipboard writer is empty.
     * Exceptions from the host clipboard callback propagate.
     */
    auto copy() -> bool;
    /** Copy the selection and erase it; return whether the document changed.
     * Return false when read-only or when the clipboard callback destroys the owner
     * or changes its document, selection, or read-only state. copy() errors propagate.
     */
    auto cut() -> bool;
    /** Read clipboard text and insert it through the character filter.
     * Return whether the document changed; read-only editors return false.
     * @throws std::logic_error No attached application clipboard.
     * @throws std::invalid_argument Empty clipboard reader or malformed UTF-8.
     * Exceptions from host callbacks propagate. Filtered-out scalars are omitted.
     */
    auto paste() -> bool;
    /** Return analyzed text extent in terminal columns and display rows, excluding the
     * line-number gutter.
     */
    auto content_size() const -> Area;
    /** Return the text viewport after any line-number gutter is removed. */
    auto viewport_size() const -> Area { return text_rect(size()).size; }
    /** Return display-column/row offsets, not UTF-8 byte offsets. */
    auto scroll_offset() const -> Point { return scroll_; }
    /** Clamp scrolling to the analyzed document, repaint and notify on effective change. */
    auto set_scroll_offset(Point position) -> bool;
    void refresh() override;
    /** Reconcile display scrolling and geometry with current text and widget size. */
    void update_viewport();
    /** Emits (offset, content extent, viewport extent) when scroll state or geometry
     * changes; units are terminal columns and rows.
     */
    sl::Signal<void(Point, Area, Area)> on_scroll;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;
    /** Handle text editing and caret navigation, ignoring disabled input and key releases.
     * Ctrl+C/X/V access the clipboard. Enter submits LineEdit; Ctrl+Enter submits
     * TextBox. Shift extends navigation selections; read_only preserves navigation and copying.
     */
    auto handle(KeyEvent const&) -> EventResponse override;
    auto handle(esc::PasteChunk const&) -> EventResponse override;
    /** Place or Shift-extend the caret with a left press, then capture for drag selection.
     * Repeated clicks select a word, then a logical line; gutter clicks select a line.
     */
    auto handle(MousePress const&) -> EventResponse override;
    auto handle(MouseMove const&) -> EventResponse override;
    auto handle(MouseRelease const&) -> EventResponse override;
    auto pointer_cancel() -> EventResponse override;
    auto focus_out() -> EventResponse override { return pointer_cancel(); }
    auto handle(MouseWheel const&) -> EventResponse override;

   protected:
    void on_arrange(Rect) override;

   private:
    TextEdit editor_;
    mutable TextLayout layout_;
    mutable TextLayout measure_layout_;
    TextLayout placeholder_layout_;
    Point scroll_ = {};
    bool dragging_ = false;
    Point last_click_ = {-1, -1};
    std::chrono::steady_clock::time_point last_click_time_ = {};
    int click_count_ = 0;
    LifetimeView<Widget> notified_owner_;
    std::uint64_t notified_revision_ = 0;
    std::pair<std::size_t, std::size_t> notified_selection_ = {};
    std::optional<std::tuple<Point, Area, Area>> notified_scroll_;
    mutable std::vector<std::size_t> line_starts_{0};
    mutable std::optional<std::uint64_t> line_revision_;
    void update_lines() const;
    auto gutter_width() const -> int;
    auto text_rect(Area) const -> Rect;
    auto text_hit(Point) const -> std::size_t;
    void update_layout() const;
    void finish();
    void notify();
};
}  // namespace ox::detail

namespace ox::detail {
template <bool Multiline>
TextControl<Multiline>::TextControl(Options x)
    : Widget{x.focus_policy, x.width_policy, x.height_policy},
      style{x.style},
      wrap{x.wrap},
      align{x.align},
      selection_style{x.selection_style},
      placeholder{std::move(x.placeholder)},
      placeholder_style{x.placeholder_style},
      focused_style{x.focused_style},
      blurred_style{x.blurred_style},
      inactive_selection_style{x.inactive_selection_style},
      line_numbers{x.line_numbers},
      highlight_line{x.highlight_line},
      gutter_style{x.gutter_style},
      current_line_style{x.current_line_style},
      read_only{x.read_only},
      accept_character{std::move(x.accept_character)},
      validator{std::move(x.validator)},
      editor_{x.text, Multiline},
      notified_owner_{track(static_cast<Widget&>(*this))},
      notified_revision_{editor_.revision()}
{}
template <bool Multiline>
void TextControl<Multiline>::update_lines() const
{
    if (!Multiline || !line_numbers ||
        (line_revision_ == editor_.revision() && !line_starts_.empty()))
        return;
    line_starts_.clear();
    line_starts_.push_back(0);
    auto const& text = editor_.text();
    for (auto at = text.find('\n'); at != std::string::npos;
         at = text.find('\n', at + 1))
        line_starts_.push_back(at + 1);
    line_revision_ = editor_.revision();
}
template <bool Multiline>
auto TextControl<Multiline>::gutter_width() const -> int
{
    if (!Multiline || !line_numbers) return 0;
    update_lines();
    auto digits = 1;
    for (auto count = line_starts_.size(); count >= 10; count /= 10)
        ++digits;
    return digits + 1;
}
template <bool Multiline>
auto TextControl<Multiline>::text_rect(Area outer) const -> Rect
{
    auto gutter = std::min(outer.width, gutter_width());
    return {{gutter, 0}, {outer.width - gutter, outer.height}};
}
template <bool Multiline>
auto TextControl<Multiline>::text_hit(Point at) const -> std::size_t
{
    auto rect = text_rect(size());
    at.x = (int)std::max((std::int64_t)std::numeric_limits<int>::min(),
                         (std::int64_t)at.x - rect.at.x);
    return text_offset(layout_, at, rect.size, align, scroll_);
}
template <bool Multiline>
void TextControl<Multiline>::update_layout() const
{ layout_.update(editor_.text(), viewport_size().width, wrap, layout_.policy()); }
template <bool Multiline>
auto TextControl<Multiline>::measure(Area available, esc::WidthPolicy policy) const
    -> Area
{
    validate(available);
    if constexpr (!Multiline) {
        if (wrap != TextWrap::None) {
            throw std::invalid_argument{"Single-line wrap must be None"};
        }
    }
    auto const interior = text_rect(available);
    measure_layout_.update(editor_.text().empty() ? placeholder : editor_.text(),
                           interior.size.width, wrap, policy);
    auto result = text_content_size(measure_layout_);
    if constexpr (!Multiline) { result.height = 1; }
    auto width = (std::int64_t)result.width + gutter_width();
    if (width > std::numeric_limits<int>::max()) throw std::length_error{"Text extent"};
    result.width = (int)width;
    return result;
}
template <bool Multiline>
void TextControl<Multiline>::finish()
{
    if (notified_revision_ != editor_.revision()) { request_layout(); }
    update_layout();
    scroll_ = text_scroll(layout_, editor_.caret(), viewport_size(), align, scroll_);
    request_repaint();
    notify();
}
template <bool Multiline>
void TextControl<Multiline>::on_arrange(Rect)
{
    layout_.update(editor_.text(), viewport_size().width, wrap, glyph_width_policy());
    if constexpr (!Multiline) {
        scroll_ =
            text_scroll(layout_, editor_.caret(), viewport_size(), align, scroll_);
        notify();
    }
    else {
        update_viewport();
        notify();
    }
}
template <bool Multiline>
auto TextControl<Multiline>::set_text(std::string_view text) -> bool
{
    auto const revision = editor_.revision();
    auto const selection = std::pair{editor_.anchor(), editor_.caret()};
    editor_.set_text(text);
    scroll_ = {};
    auto const changed = revision != editor_.revision();
    if (changed || selection != std::pair{editor_.anchor(), editor_.caret()}) {
        finish();
    }
    return changed;
}
template <bool Multiline>
auto TextControl<Multiline>::edit(std::function<void(TextEdit&)> action) -> bool
{
    if (!action) { throw std::invalid_argument{"Editing action is empty"}; }
    auto staged = editor_;
    auto const revision = editor_.revision();
    auto const selection = std::pair{editor_.anchor(), editor_.caret()};
    auto lifetime = track(*this);
    action(staged);
    if (staged.multiline() != Multiline) {
        throw std::invalid_argument{"Editor mode cannot change"};
    }
    auto* widget = lifetime.try_get();
    if (!widget) { return false; }
    if (revision != widget->editor_.revision() ||
        selection != std::pair{widget->editor_.anchor(), widget->editor_.caret()}) {
        throw std::logic_error{"Editing action changed its owner"};
    }
    auto const changed = staged.text() != widget->text() ||
                         selection != std::pair{staged.anchor(), staged.caret()};
    widget->editor_ = std::move(staged);
    if (changed) { widget->finish(); }
    return changed;
}
template <bool Multiline>
auto TextControl<Multiline>::set_caret(std::size_t at, bool extend) -> bool
{
    auto const changed = editor_.set_caret(at, extend);
    if (changed) { finish(); }
    return changed;
}
template <bool Multiline>
auto TextControl<Multiline>::select_all() -> bool
{
    auto const changed = editor_.select_all();
    if (changed) { finish(); }
    return changed;
}
template <bool Multiline>
auto TextControl<Multiline>::insert(std::string_view text) -> bool
{
    auto const changed = editor_.insert(text, accept_character);
    if (changed) { finish(); }
    return changed;
}
template <bool Multiline>
auto TextControl<Multiline>::append_lines(std::string_view text, std::size_t discard)
    -> bool
    requires Multiline
{
    update_layout();
    auto retained = editor_.text().size() - std::min(discard, editor_.text().size());
    // Only stream a measured snapshot of this document, never placeholder text.
    auto measured = !editor_.text().empty() && measure_layout_.text() == editor_.text();
    auto changed = editor_.append_lines(text, discard);
    if (changed) {
        auto suffix = std::string_view{editor_.text()}.substr(retained);
        layout_.append_lines(suffix, discard);
        if (measured) { measure_layout_.append_lines(suffix, discard); }
        finish();
    }
    return changed;
}
template <bool Multiline>
auto TextControl<Multiline>::undo() -> bool
{
    auto const changed = editor_.undo();
    if (changed) { finish(); }
    return changed;
}
template <bool Multiline>
auto TextControl<Multiline>::redo() -> bool
{
    auto const changed = editor_.redo();
    if (changed) { finish(); }
    return changed;
}
template <bool Multiline>
auto TextControl<Multiline>::submit() -> bool
{
    if (!is_enabled()) { return false; }
    auto const value = editor_.text();
    auto check = validator;
    auto lifetime = track(*this);
    auto const valid = !check || check(value);
    auto* widget = lifetime.try_get();
    if (!widget || widget->text() != value) { return false; }
    if (!valid) {
        widget->on_invalid(value);
        return false;
    }
    widget->on_submit(value);
    return true;
}
template <bool Multiline>
void TextControl<Multiline>::notify()
{
    auto lifetime = track(*this);
    if (notified_owner_ != lifetime) {
        notified_owner_ = track(static_cast<Widget&>(*this));
        notified_revision_ = std::numeric_limits<std::uint64_t>::max();
        notified_selection_ = {};
        notified_scroll_.reset();
    }
    if (notified_revision_ != editor_.revision()) {
        notified_revision_ = editor_.revision();
        if (!on_change.is_empty()) {
            auto const value = editor_.text();
            on_change(value);
        }
    }
    auto* widget = lifetime.try_get();
    if (!widget) { return; }
    auto const selection = std::pair{widget->editor_.anchor(), widget->editor_.caret()};
    if (widget->notified_selection_ != selection) {
        widget->notified_selection_ = selection;
        widget->on_selection_change(selection.first, selection.second);
    }
    widget = lifetime.try_get();
    if (!widget) { return; }
    auto const value =
        std::tuple{widget->scroll_, widget->content_size(), widget->viewport_size()};
    if (widget->notified_scroll_ != value) {
        widget->notified_scroll_ = value;
        widget->on_scroll(std::get<0>(value), std::get<1>(value), std::get<2>(value));
    }
}
template <bool Multiline>
auto TextControl<Multiline>::copy() -> bool
{
    auto const [begin, end] = editor_.selection();
    if (begin == end) { return false; }
    auto const value = editor_.text().substr(begin, end - begin);
    auto write = clipboard().write;
    if (!write) { throw std::invalid_argument{"Clipboard writer is empty"}; }
    write(value);
    return true;
}
template <bool Multiline>
auto TextControl<Multiline>::cut() -> bool
{
    if (read_only) { return false; }
    auto lifetime = track(*this);
    auto const selection = editor_.selection();
    auto const revision = editor_.revision();
    if (!copy()) { return false; }
    auto* widget = lifetime.try_get();
    if (!widget || widget->read_only || widget->editor_.selection() != selection ||
        widget->editor_.revision() != revision) {
        return false;
    }
    auto const changed = widget->editor_.erase_backward();
    if (changed) { widget->finish(); }
    return changed;
}
template <bool Multiline>
auto TextControl<Multiline>::paste() -> bool
{
    if (read_only) { return false; }
    auto read = clipboard().read;
    if (!read) { throw std::invalid_argument{"Clipboard reader is empty"}; }
    auto lifetime = track(*this);
    auto const text = read();
    if (auto* widget = lifetime.try_get(); widget && !widget->read_only) {
        return widget->insert(text);
    }
    return false;
}
template <bool Multiline>
auto TextControl<Multiline>::handle(KeyEvent const& key) -> EventResponse
{
    if (!is_enabled() || key.action == KeyAction::Release) { return {}; }
    click_count_ = 0;
    auto matches = [&](auto code, Modifiers modifiers = {}) {
        return key.matches(code, modifiers, key.action);
    };
    if (matches(U'c', {.ctrl = true})) {
        copy();
        return {.handled = true};
    }
    if (matches(U'x', {.ctrl = true})) {
        auto const changed = cut();
        return {.handled = true, .repaint = changed};
    }
    if (matches(U'v', {.ctrl = true})) {
        auto const changed = paste();
        return {.handled = true, .repaint = changed};
    }
    if (matches(KeyCode::Enter, {.ctrl = Multiline}) &&
        key.action == KeyAction::Press) {
        submit();
        return {.handled = true};
    }
    if constexpr (Multiline) {
        if (matches(KeyCode::PageUp) || matches(KeyCode::PageDown)) {
            update_layout();
            auto const changed = editor_.move_vertical(layout_,
                                                       matches(KeyCode::PageUp)
                                                           ? -std::max(size().height, 1)
                                                           : std::max(size().height, 1),
                                                       key.modifiers.shift);
            if (changed) { finish(); }
            return {.handled = true, .repaint = changed};
        }
    }
    auto const response = edit_text(editor_, layout_, scroll_, viewport_size(), wrap,
                                    align, key, accept_character, read_only);
    if (response.repaint) { finish(); }
    return response;
}
template <bool Multiline>
auto TextControl<Multiline>::handle(esc::PasteChunk const& chunk) -> EventResponse
{
    auto const response = edit_text(editor_, layout_, scroll_, viewport_size(), wrap,
                                    align, chunk, accept_character, read_only);
    if (response.repaint) { finish(); }
    return response;
}
template <bool Multiline>
auto TextControl<Multiline>::handle(MousePress const& event) -> EventResponse
{
    if (!is_enabled() || event.button != MouseButton::Left) { return {}; }
    update_layout();
    if (event.at.x >= 0 && event.at.x < text_rect(size()).at.x) {
        auto row = std::clamp((std::int64_t)event.at.y + scroll_.y, std::int64_t{0},
                              (std::int64_t)layout_.lines().size() - 1);
        auto changed = editor_.select_line(layout_.lines()[(std::size_t)row].begin);
        if (changed) { finish(); }
        return {.handled = true, .repaint = changed};
    }
    auto const at = text_hit(event.at);
    auto const now = std::chrono::steady_clock::now();
    click_count_ = !event.modifiers.shift && event.at == last_click_ &&
                           now - last_click_time_ < std::chrono::milliseconds{400}
                       ? click_count_ % 3 + 1
                       : 1;
    last_click_ = event.at;
    last_click_time_ = now;
    auto changed = editor_.set_caret(at, event.modifiers.shift);
    if (!event.modifiers.shift && click_count_ == 2) {
        changed |= editor_.select_word(at);
    }
    if (!event.modifiers.shift && click_count_ == 3) {
        changed |= editor_.select_line(at);
    }
    dragging_ = true;
    capture_pointer();
    if (changed) {
        request_repaint();
        notify();
    }
    return {.handled = true, .repaint = changed};
}
template <bool Multiline>
auto TextControl<Multiline>::handle(MouseMove const& event) -> EventResponse
{
    if (!dragging_ || !is_pointer_captured()) { return {}; }
    if (event.held_button && event.held_button != MouseButton::Left) { return {}; }
    update_layout();
    auto const changed = editor_.set_caret(text_hit(event.at), true);
    if (changed) { finish(); }
    return {.handled = true, .repaint = changed};
}
template <bool Multiline>
auto TextControl<Multiline>::handle(MouseRelease const& event) -> EventResponse
{
    if (event.button && event.button != MouseButton::Left) { return {}; }
    auto const dragging = dragging_;
    dragging_ = false;
    release_pointer();
    return {.handled = dragging};
}
template <bool Multiline>
auto TextControl<Multiline>::pointer_cancel() -> EventResponse
{
    dragging_ = false;
    release_pointer();
    return {};
}
template <bool Multiline>
auto TextControl<Multiline>::content_size() const -> Area
{
    update_layout();
    auto result = text_content_size(layout_);
    if (result.width < std::numeric_limits<int>::max()) { ++result.width; }
    if (align == Align::Right && result.width <= viewport_size().width &&
        viewport_size().width < std::numeric_limits<int>::max()) {
        result.width = viewport_size().width + 1;
    }
    return result;
}
template <bool Multiline>
void TextControl<Multiline>::refresh()
{
    request_layout();
    update_viewport();
    request_repaint();
    notify();
}
template <bool Multiline>
void TextControl<Multiline>::update_viewport()
{
    auto const content = content_size();
    scroll_ = {clamp_scroll(scroll_.x, content.width, viewport_size().width),
               clamp_scroll(scroll_.y, content.height, viewport_size().height)};
}
template <bool Multiline>
auto TextControl<Multiline>::set_scroll_offset(Point at) -> bool
{
    auto const content = content_size();
    auto const next = Point{clamp_scroll(at.x, content.width, viewport_size().width),
                            clamp_scroll(at.y, content.height, viewport_size().height)};
    if (next == scroll_) { return false; }
    scroll_ = next;
    request_repaint();
    notify();
    return true;
}
template <bool Multiline>
auto TextControl<Multiline>::handle(MouseWheel const& event) -> EventResponse
{
    auto const content = content_size();
    auto const changed = set_scroll_offset(
        {scroll_by(scroll_.x, event.dx, content.width, viewport_size().width),
         scroll_by(scroll_.y, event.dy, content.height, viewport_size().height)});
    return {.handled = changed, .repaint = changed};
}
template <bool Multiline>
auto TextControl<Multiline>::paint(Canvas canvas) -> Cursor
{
    auto rect = text_rect(canvas.size());
    if constexpr (!Multiline) rect.size.height = std::min(rect.size.height, 1);
    auto view = canvas.subcanvas(rect.at, rect.size);
    layout_.update(editor_.text(), view.size().width, wrap, canvas.width_policy());
    auto content = text_content_size(layout_);
    if (content.width < std::numeric_limits<int>::max()) { ++content.width; }
    if (align == Align::Right && content.width <= view.size().width &&
        view.size().width < std::numeric_limits<int>::max()) {
        content.width = view.size().width + 1;
    }
    scroll_ = {clamp_scroll(scroll_.x, content.width, view.size().width),
               clamp_scroll(scroll_.y, content.height, view.size().height)};
    auto normal = appearance(style | (is_focused() ? focused_style : blurred_style),
                             Surface::Raised);
    if constexpr (!Multiline) {
        if (theme().styled && is_focused()) {
            normal.traits.insert(Trait::Underline);
            if (style.foreground == Color{default_color} && !focused_style.foreground)
                normal.foreground = theme().palette.accent;
        }
    }
    fill(canvas, U' ' | normal);
    auto selected = theme_selection(normal, selection_style, theme(), is_focused());
    if (!is_focused()) selected = selected | inactive_selection_style;
    if (editor_.text().empty() && !placeholder.empty()) {
        placeholder_layout_.update(placeholder, view.size().width, wrap,
                                   canvas.width_policy());
        auto placeholder = normal | placeholder_style;
        if (theme().styled &&
            placeholder_style.foreground == std::optional<Color>{colors::bright_black})
            placeholder.foreground = theme().palette.muted;
        paint_text(view, placeholder_layout_, align, {}, placeholder, placeholder);
    }
    else {
        paint_text(view, layout_, align, scroll_, normal, selected,
                   editor_.selection());
        if (highlight_line && is_focused()) {
            auto row = layout_.position(editor_.caret()).y;
            auto active = normal;
            if (theme().styled)
                active.background =
                    mix_color(normal.background, theme().palette.selection, .4);
            active = active | current_line_style;
            fill(view.subcanvas({0, row - scroll_.y}, {view.size().width, 1}), active);
            auto at = text_position(layout_, layout_.lines()[(std::size_t)row].begin,
                                    view.size(), align, scroll_);
            put_text_line(view, at, layout_, (std::size_t)row, active, selected,
                          editor_.selection());
        }
    }
    if (rect.at.x) {
        auto gutter = canvas.subcanvas({}, {rect.at.x, canvas.size().height});
        auto muted = appearance(style, Surface::Muted) | gutter_style;
        fill(gutter, muted);
        auto clip = gutter.visible();
        auto end = std::min((std::int64_t)layout_.lines().size(),
                            (std::int64_t)scroll_.y + clip.at.y + clip.size.height);
        auto current = std::upper_bound(line_starts_.begin(), line_starts_.end(),
                                        editor_.caret()) -
                       line_starts_.begin();
        for (auto row = (std::int64_t)scroll_.y + clip.at.y; row < end; ++row) {
            auto begin = layout_.lines()[(std::size_t)row].begin;
            auto logical =
                std::upper_bound(line_starts_.begin(), line_starts_.end(), begin) -
                line_starts_.begin();
            auto label = begin == line_starts_[(std::size_t)logical - 1]
                             ? std::to_string(logical)
                         : theme().ascii ? ">"
                                         : "↪";
            auto number_style = muted;
            if (is_focused() && current == logical && theme().styled)
                number_style.foreground = theme().palette.accent;
            put_label(gutter.subcanvas({0, (int)row - scroll_.y},
                                       {std::max(0, rect.at.x - 1), 1}),
                      label, Align::Right, number_style);
        }
    }
    auto const local =
        text_position(layout_, editor_.caret(), view.size(), align, scroll_);
    return view.contains(local) ? Cursor{translate(local, rect.at)} : Cursor{};
}
}  // namespace ox::detail
