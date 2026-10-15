#pragma once
#include <algorithm>
#include <functional>
#include <ox/clipboard.hpp>
#include <ox/put.hpp>
#include <ox/scroll.hpp>
#include <ox/text_view.hpp>
#include <ox/widget.hpp>
#include <string>
#include <tuple>
namespace ox::detail {
template <bool Multiline>
class TextControl : public Widget {
   public:
    using Widget::handle;
    struct Options {
        std::string text = {};
        TextWrap wrap = Multiline ? TextWrap::Word : TextWrap::None;
        Align align = Align::Left;
        Style style = {};
        StylePatch selection_style = {.background = {},
                                      .foreground = {},
                                      .enabled = Trait::Inverse,
                                      .disabled = {}};
        std::string placeholder = {};
        StylePatch placeholder_style = {.background = {},
                                        .foreground = colors::bright_black,
                                        .enabled = {},
                                        .disabled = {}};
        bool read_only = false;
        std::function<bool(char32_t)> accept_character = {};
        std::function<bool(std::string_view)> validator = {};
        FocusPolicy focus_policy = FocusPolicy::Strong;
        SizePolicy width_policy = SizePolicy::flex();
        SizePolicy height_policy =
            Multiline ? SizePolicy::flex() : SizePolicy::fixed(1);
    };
    Style style;
    TextWrap wrap;
    Align align;
    StylePatch selection_style;
    std::string placeholder;
    StylePatch placeholder_style;
    bool read_only;
    std::function<bool(char32_t)> accept_character;
    /** Validate complete values on submit; intermediate editing remains allowed. */
    std::function<bool(std::string_view)> validator;
    sl::Signal<void(std::string const&)> on_change;
    sl::Signal<void(std::size_t, std::size_t)> on_selection_change;
    sl::Signal<void(std::string const&)> on_submit;
    sl::Signal<void(std::string const&)> on_invalid;
    explicit TextControl(Options x);
    auto text() const -> std::string const& { return editor_.text(); }
    auto editor() const -> TextEdit const& { return editor_; }
    auto set_text(std::string_view text) -> bool;
    /** Stage a batch of programmatic edits; mode must remain unchanged. */
    auto edit(std::function<void(TextEdit&)> action) -> bool;
    auto set_caret(std::size_t offset, bool extend = false) -> bool;
    auto select_all() -> bool;
    auto insert(std::string_view text) -> bool;
    /** Stream complete lines without copying the editor; preserve selection. */
    auto append_lines(std::string_view text, std::size_t discard_prefix = 0) -> bool
        requires Multiline;
    auto undo() -> bool;
    auto redo() -> bool;
    auto submit() -> bool;
    auto copy() -> bool;
    auto cut() -> bool;
    auto paste() -> bool;
    auto content_size() const -> Area;
    auto viewport_size() const -> Area { return size(); }
    auto scroll_offset() const -> Point { return scroll_; }
    auto set_scroll_offset(Point position) -> bool;
    void refresh() override;
    void update_viewport();
    sl::Signal<void(Point, Area, Area)> on_scroll;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;
    auto handle(KeyEvent const&) -> EventResponse override;
    auto handle(esc::PasteChunk const&) -> EventResponse override;
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
      read_only{x.read_only},
      accept_character{std::move(x.accept_character)},
      validator{std::move(x.validator)},
      editor_{x.text, Multiline},
      notified_owner_{track(static_cast<Widget&>(*this))},
      notified_revision_{editor_.revision()}
{}
template <bool Multiline>
void TextControl<Multiline>::update_layout() const
{ layout_.update(editor_.text(), size().width, wrap, layout_.policy()); }
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
    measure_layout_.update(editor_.text().empty() ? placeholder : editor_.text(),
                           available.width, wrap, policy);
    auto result = text_content_size(measure_layout_);
    if constexpr (!Multiline) { result.height = 1; }
    return result;
}
template <bool Multiline>
void TextControl<Multiline>::finish()
{
    if (notified_revision_ != editor_.revision()) { request_layout(); }
    update_layout();
    scroll_ = text_scroll(layout_, editor_.caret(), size(), align, scroll_);
    request_repaint();
    notify();
}
template <bool Multiline>
void TextControl<Multiline>::on_arrange(Rect)
{
    layout_.update(editor_.text(), size().width, wrap, glyph_width_policy());
    if constexpr (!Multiline) {
        scroll_ = text_scroll(layout_, editor_.caret(), size(), align, scroll_);
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
        std::tuple{widget->scroll_, widget->content_size(), widget->size()};
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
    auto const response = edit_text(editor_, layout_, scroll_, size(), wrap, align, key,
                                    accept_character, read_only);
    if (response.repaint) {
        if (notified_revision_ != editor_.revision()) { request_layout(); }
        request_repaint();
        notify();
    }
    return response;
}
template <bool Multiline>
auto TextControl<Multiline>::handle(esc::PasteChunk const& chunk) -> EventResponse
{
    auto const response = edit_text(editor_, layout_, scroll_, size(), wrap, align,
                                    chunk, accept_character, read_only);
    if (response.repaint) {
        if (notified_revision_ != editor_.revision()) { request_layout(); }
        request_repaint();
        notify();
    }
    return response;
}
template <bool Multiline>
auto TextControl<Multiline>::handle(MousePress const& event) -> EventResponse
{
    if (!is_enabled() || event.button != MouseButton::Left) { return {}; }
    update_layout();
    auto const at = text_offset(layout_, event.at, size(), align, scroll_);
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
    auto const changed =
        editor_.set_caret(text_offset(layout_, event.at, size(), align, scroll_), true);
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
    if (align == Align::Right && result.width <= size().width &&
        size().width < std::numeric_limits<int>::max()) {
        result.width = size().width + 1;
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
    scroll_ = {clamp_scroll(scroll_.x, content.width, size().width),
               clamp_scroll(scroll_.y, content.height, size().height)};
}
template <bool Multiline>
auto TextControl<Multiline>::set_scroll_offset(Point at) -> bool
{
    auto const content = content_size();
    auto const next = Point{clamp_scroll(at.x, content.width, size().width),
                            clamp_scroll(at.y, content.height, size().height)};
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
        {scroll_by(scroll_.x, event.dx, content.width, size().width),
         scroll_by(scroll_.y, event.dy, content.height, size().height)});
    return {.handled = changed, .repaint = changed};
}
template <bool Multiline>
auto TextControl<Multiline>::paint(Canvas canvas) -> Cursor
{
    layout_.update(editor_.text(), canvas.size().width, wrap, canvas.width_policy());
    auto content = text_content_size(layout_);
    if (content.width < std::numeric_limits<int>::max()) { ++content.width; }
    if (align == Align::Right && content.width <= canvas.size().width &&
        canvas.size().width < std::numeric_limits<int>::max()) {
        content.width = canvas.size().width + 1;
    }
    scroll_ = {clamp_scroll(scroll_.x, content.width, canvas.size().width),
               clamp_scroll(scroll_.y, content.height, canvas.size().height)};
    auto normal = appearance(style, Surface::Raised);
    if constexpr (!Multiline) {
        if (theme().styled && is_focused()) {
            normal.traits.insert(Trait::Underline);
            if (style.foreground == Color{default_color})
                normal.foreground = theme().palette.accent;
        }
    }
    fill(canvas, U' ' | normal);
    auto view = canvas.subcanvas(
        {}, {canvas.size().width,
             Multiline ? canvas.size().height : std::min(canvas.size().height, 1)});
    if (editor_.text().empty() && !placeholder.empty()) {
        placeholder_layout_.update(placeholder, canvas.size().width, wrap,
                                   canvas.width_policy());
        auto placeholder = normal | placeholder_style;
        if (theme().styled &&
            placeholder_style.foreground == std::optional<Color>{colors::bright_black})
            placeholder.foreground = theme().palette.muted;
        paint_text(view, placeholder_layout_, align, {}, placeholder, placeholder);
    }
    else {
        paint_text(view, layout_, align, scroll_, normal,
                   selection_appearance(normal, selection_style), editor_.selection());
    }
    auto const point =
        text_position(layout_, editor_.caret(), canvas.size(), align, scroll_);
    return canvas.contains(point) ? Cursor{point} : Cursor{};
}
}  // namespace ox::detail
