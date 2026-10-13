#include <ox/core/text.hpp>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>

#include <esc/text.hpp>

namespace ox {
namespace {
auto normalize(std::string_view text,
               bool multiline,
               std::function<bool(char32_t)> const& accept = {}) -> std::string
{
    auto decoded = esc::utf8_to_utf32(text);
    auto result = std::u32string{};
    result.reserve(decoded.size());
    for (auto i = std::size_t{0}; i < decoded.size(); ++i) {
        auto c = decoded[i];
        if (c == U'\r') {
            c = U'\n';
            if (i + 1 < decoded.size() && decoded[i + 1] == U'\n') { ++i; }
        }
        if ((c < U' ' && c != U'\n' && c != U'\t') || (c >= U'\x7f' && c <= U'\x9f')) {
            throw std::invalid_argument{"Text contains a terminal control character"};
        }
        if (!multiline && (c == U'\n' || c == U'\t')) { continue; }
        if (!accept || accept(c)) { result.push_back(c); }
    }
    return esc::utf32_to_utf8(result);
}

auto grapheme_boundaries(std::string_view text) -> std::vector<std::size_t>
{
    auto result = std::vector<std::size_t>{0};
    auto offset = std::size_t{0};
    for (auto cluster : esc::graphemes(text)) {
        offset += cluster.size();
        result.push_back(offset);
    }
    return result;
}

auto columns(std::string_view cluster, int column, esc::WidthPolicy policy, int tabs)
    -> int
{
    if (cluster == "\t") { return tabs - column % tabs; }
    auto width = esc::grapheme_width(cluster, policy);
    if (!width) { throw std::invalid_argument{"Text requires known grapheme widths"}; }
    if (*width == 0) {
        auto display = std::u32string{U'◌'} + esc::utf8_to_utf32(cluster);
        width = esc::grapheme_width(display, policy);
        if (!width || *width == 0) { throw std::invalid_argument{"Unrenderable mark"}; }
    }
    if (*width > (std::size_t)std::numeric_limits<int>::max()) {
        throw std::length_error{"Text column count overflow"};
    }
    return (int)*width;
}

auto add_columns(int a, int b) -> int
{
    if (b > std::numeric_limits<int>::max() - a) {
        throw std::length_error{"Text column count overflow"};
    }
    return a + b;
}

auto same_policy(esc::WidthPolicy a, esc::WidthPolicy b) -> bool
{
    return a.ambiguous == b.ambiguous && a.emoji == b.emoji &&
           a.clusters == b.clusters && a.orphan_marks == b.orphan_marks &&
           a.unassigned == b.unassigned &&
           std::ranges::equal(a.overrides, b.overrides, [](auto x, auto y) {
               return x.scalar == y.scalar && x.cells == y.cells;
           });
}

auto whitespace(std::string_view cluster) -> bool
{ return cluster == " " || cluster == "\t" || cluster == "\n"; }
}  // namespace

auto display_grapheme(std::string_view cluster,
                      int column,
                      esc::WidthPolicy policy,
                      int tab_width) -> std::u32string
{
    if (column < 0 || tab_width <= 0) {
        throw std::invalid_argument{"Invalid text geometry"};
    }
    if (cluster == "\t") {
        if (esc::grapheme_width(" ", policy) != 1) {
            throw std::invalid_argument{"Tabs require one-column spaces"};
        }
        return std::u32string((std::size_t)(tab_width - column % tab_width), U' ');
    }
    auto const width = esc::grapheme_width(cluster, policy);
    if (!width || cluster.empty()) {
        throw std::invalid_argument{"Unprintable grapheme"};
    }
    auto result = esc::utf8_to_utf32(cluster);
    if (*width == 0) {
        result.insert(result.begin(), U'◌');
        auto const display_width = esc::grapheme_width(result, policy);
        if (!display_width || *display_width == 0) {
            throw std::invalid_argument{"Unrenderable mark"};
        }
    }
    return result;
}

auto layout_text(std::string_view text,
                 int width,
                 TextWrap wrap,
                 esc::WidthPolicy policy,
                 int tab_width) -> std::vector<TextLine>
{
    if (width < 0 || tab_width <= 0 ||
        (wrap != TextWrap::None && wrap != TextWrap::Any && wrap != TextWrap::Word)) {
        throw std::invalid_argument{"Invalid text layout"};
    }
    // Validate the entire input, including text beyond the viewport.
    (void)normalize(text, true);
    if (esc::grapheme_width(" ", policy) != 1) {
        throw std::invalid_argument{"Text layout requires one-column spaces"};
    }
    auto lines = std::vector<TextLine>{};
    auto begin = std::size_t{0};
    while (begin < text.size()) {
        auto end = begin;
        auto next = begin;
        auto used = 0;
        auto word_end = begin;
        auto word_columns = 0;
        for (auto cluster : esc::graphemes(text.substr(begin))) {
            if (cluster == "\n" || cluster == "\r" || cluster == "\r\n") {
                next = end + cluster.size();
                break;
            }
            auto const cells = columns(cluster, used, policy, tab_width);
            if (wrap != TextWrap::None && width > 0 &&
                cells > width - std::min(used, width) && end != begin) {
                if (wrap == TextWrap::Word && word_end > begin) {
                    end = word_end;
                    used = word_columns;
                }
                next = end;
                break;
            }
            used = add_columns(used, cells);
            end += cluster.size();
            next = end;
            if (whitespace(cluster)) {
                word_end = end;
                word_columns = used;
            }
        }
        lines.push_back({begin, end, next, used});
        begin = next;
    }
    if (lines.empty() || lines.back().next > lines.back().end ||
        (wrap != TextWrap::None && width > 0 && lines.back().columns >= width)) {
        lines.push_back({text.size(), text.size(), text.size(), 0});
    }
    if (lines.size() > (std::size_t)std::numeric_limits<int>::max()) {
        throw std::length_error{"Too many text rows"};
    }
    return lines;
}

void swap(TextLayout& a, TextLayout& b) noexcept
{
    using std::swap;
    swap(a.text_, b.text_);
    swap(a.lines_, b.lines_);
    swap(a.boundaries_, b.boundaries_);
    swap(a.width_, b.width_);
    swap(a.wrap_, b.wrap_);
    swap(a.policy_, b.policy_);
    swap(a.overrides_, b.overrides_);
    swap(a.tab_width_, b.tab_width_);
}
TextLayout::TextLayout(TextLayout&& other) : TextLayout{} { swap(*this, other); }
auto TextLayout::operator=(TextLayout other) -> TextLayout&
{
    swap(*this, other);
    return *this;
}

auto TextLayout::policy() const -> esc::WidthPolicy
{
    auto result = policy_;
    result.overrides = overrides_;
    return result;
}

auto TextLayout::update(std::string_view text,
                        int width,
                        TextWrap wrap,
                        esc::WidthPolicy policy,
                        int tab_width) -> bool
{
    if (width_ >= 0 && width_ == width && wrap_ == wrap && tab_width_ == tab_width &&
        text_ == text && same_policy(this->policy(), policy)) {
        return false;
    }
    auto lines = layout_text(text, width, wrap, policy, tab_width);
    auto breaks = grapheme_boundaries(text);
    auto owned_text = std::string{text};
    auto overrides = std::vector<esc::ScalarWidthOverride>{policy.overrides.begin(),
                                                           policy.overrides.end()};
    text_.swap(owned_text);
    lines_.swap(lines);
    boundaries_.swap(breaks);
    overrides_.swap(overrides);
    policy_ = policy;
    policy_.overrides = {};
    width_ = width;
    wrap_ = wrap;
    tab_width_ = tab_width;
    return true;
}

auto TextLayout::position(std::size_t offset) const -> Point
{
    if (!std::ranges::binary_search(boundaries_, offset)) {
        throw std::invalid_argument{"Caret is not a grapheme boundary"};
    }
    auto y = std::size_t{0};
    while (y + 1 < lines_.size() && offset >= lines_[y].next) {
        ++y;
    }
    auto x = 0;
    auto const& line = lines_[y];
    for (auto cluster : esc::graphemes(
             std::string_view{text_}.substr(line.begin, offset - line.begin))) {
        if (cluster == "\n" || cluster == "\r" || cluster == "\r\n") { break; }
        x = add_columns(x, columns(cluster, x, policy(), tab_width_));
    }
    return {x, (int)y};
}

auto TextLayout::offset(Point point) const -> std::size_t
{
    auto const y = std::clamp(point.y, 0, (int)lines_.size() - 1);
    auto const& line = lines_[(std::size_t)y];
    auto x = 0;
    auto at = line.begin;
    for (auto cluster : esc::graphemes(
             std::string_view{text_}.substr(line.begin, line.end - line.begin))) {
        auto const end = add_columns(x, columns(cluster, x, policy(), tab_width_));
        if (point.x < end) { return at; }
        at += cluster.size();
        x = end;
    }
    return line.end;
}

TextEdit::TextEdit(std::string_view text, bool multiline, std::size_t history_limit)
    : multiline_{multiline}, history_limit_{history_limit}
{ set_text(text); }

void swap(TextEdit& a, TextEdit& b) noexcept
{
    using std::swap;
    swap(a.text_, b.text_);
    swap(a.boundaries_, b.boundaries_);
    swap(a.caret_, b.caret_);
    swap(a.anchor_, b.anchor_);
    swap(a.multiline_, b.multiline_);
    swap(a.history_limit_, b.history_limit_);
    swap(a.undo_, b.undo_);
    swap(a.redo_, b.redo_);
    swap(a.preferred_column_, b.preferred_column_);
    swap(a.paste_, b.paste_);
    swap(a.pasting_, b.pasting_);
}
TextEdit::TextEdit(TextEdit&& other)
    : TextEdit{{}, other.multiline_, other.history_limit_}
{ swap(*this, other); }
auto TextEdit::operator=(TextEdit other) -> TextEdit&
{
    swap(*this, other);
    return *this;
}

void TextEdit::set_text(std::string_view text)
{
    auto value = normalize(text, multiline_);
    auto breaks = grapheme_boundaries(value);
    text_.swap(value);
    boundaries_.swap(breaks);
    caret_ = anchor_ = 0;
    preferred_column_.reset();
    undo_.clear();
    redo_.clear();
    paste_.clear();
    pasting_ = false;
}

auto TextEdit::selection() const -> std::pair<std::size_t, std::size_t>
{ return std::minmax(anchor_, caret_); }

auto TextEdit::set_caret(std::size_t offset, bool extend) -> bool
{
    if (!std::ranges::binary_search(boundaries_, offset)) {
        throw std::invalid_argument{"Caret is not a grapheme boundary"};
    }
    auto const changed = caret_ != offset || (!extend && anchor_ != offset);
    caret_ = offset;
    if (!extend) { anchor_ = offset; }
    preferred_column_.reset();
    return changed;
}

auto TextEdit::select_all() -> bool
{
    auto const changed = anchor_ != 0 || caret_ != text_.size();
    anchor_ = 0;
    caret_ = text_.size();
    preferred_column_.reset();
    return changed;
}

auto TextEdit::word_left(std::size_t at) const -> std::size_t
{
    auto i =
        (std::size_t)(std::ranges::lower_bound(boundaries_, at) - boundaries_.begin());
    auto space = [&](std::size_t n) {
        return whitespace(std::string_view{text_}.substr(
            boundaries_[n], boundaries_[n + 1] - boundaries_[n]));
    };
    while (i > 0 && space(i - 1)) {
        --i;
    }
    while (i > 0 && !space(i - 1)) {
        --i;
    }
    return boundaries_[i];
}
auto TextEdit::word_right(std::size_t at) const -> std::size_t
{
    auto i =
        (std::size_t)(std::ranges::lower_bound(boundaries_, at) - boundaries_.begin());
    auto space = [&](std::size_t n) {
        return whitespace(std::string_view{text_}.substr(
            boundaries_[n], boundaries_[n + 1] - boundaries_[n]));
    };
    while (i + 1 < boundaries_.size() && !space(i)) {
        ++i;
    }
    while (i + 1 < boundaries_.size() && space(i)) {
        ++i;
    }
    return boundaries_[i];
}

auto TextEdit::move_left(bool extend, bool word) -> bool
{
    auto const [begin, end] = selection();
    if (!extend && begin != end) { return set_caret(begin); }
    auto const index =
        std::ranges::lower_bound(boundaries_, caret_) - boundaries_.begin();
    return set_caret(
        word ? word_left(caret_)
             : boundaries_[(std::size_t)std::max(index - 1, decltype(index){0})],
        extend);
}
auto TextEdit::move_right(bool extend, bool word) -> bool
{
    auto const [begin, end] = selection();
    if (!extend && begin != end) { return set_caret(end); }
    auto const index = (std::size_t)(std::ranges::lower_bound(boundaries_, caret_) -
                                     boundaries_.begin());
    return set_caret(word ? word_right(caret_)
                          : boundaries_[std::min(index + 1, boundaries_.size() - 1)],
                     extend);
}

auto TextEdit::move_vertical(TextLayout const& layout, int rows, bool extend) -> bool
{
    if (layout.text() != text_) { throw std::invalid_argument{"Stale text layout"}; }
    auto const current = layout.position(caret_);
    auto const column = preferred_column_.value_or(current.x);
    auto const y = std::clamp(std::int64_t{current.y} + rows, std::int64_t{0},
                              (std::int64_t)layout.lines().size() - 1);
    auto const changed = set_caret(layout.offset({column, (int)y}), extend);
    preferred_column_ = column;
    return changed;
}

auto TextEdit::replace(std::size_t begin, std::size_t end, std::string value) -> bool
{
    auto next = text_;
    next.replace(begin, end - begin, value);
    auto breaks = grapheme_boundaries(next);
    auto const after = *std::ranges::lower_bound(breaks, begin + value.size());
    if (next == text_ && anchor_ == after && caret_ == after) { return false; }
    auto change = Change{
        begin, text_.substr(begin, end - begin), std::move(value), anchor_, caret_,
        after};
    if (history_limit_ > 0) { undo_.push_back(std::move(change)); }
    text_.swap(next);
    boundaries_.swap(breaks);
    anchor_ = caret_ = after;
    preferred_column_.reset();
    redo_.clear();
    if (undo_.size() > history_limit_) { undo_.erase(undo_.begin()); }
    return true;
}

auto TextEdit::insert(std::string_view text,
                      std::function<bool(char32_t)> const& accept) -> bool
{
    auto value = normalize(text, multiline_, accept);
    auto const [begin, end] = selection();
    // A rejected/empty insertion preserves a selection; deletion is explicit.
    if (value.empty()) { return false; }
    return replace(begin, end, std::move(value));
}

auto TextEdit::erase_backward(bool word) -> bool
{
    auto [begin, end] = selection();
    if (begin == end) {
        auto const index =
            std::ranges::lower_bound(boundaries_, caret_) - boundaries_.begin();
        begin = word
                    ? word_left(caret_)
                    : boundaries_[(std::size_t)std::max(index - 1, decltype(index){0})];
    }
    return begin != end && replace(begin, end, {});
}
auto TextEdit::erase_forward(bool word) -> bool
{
    auto [begin, end] = selection();
    if (begin == end) {
        auto const index = (std::size_t)(std::ranges::lower_bound(boundaries_, caret_) -
                                         boundaries_.begin());
        end = word ? word_right(caret_)
                   : boundaries_[std::min(index + 1, boundaries_.size() - 1)];
    }
    return begin != end && replace(begin, end, {});
}

auto TextEdit::replay(bool backward) -> bool
{
    auto& source = backward ? undo_ : redo_;
    auto& destination = backward ? redo_ : undo_;
    if (source.empty()) { return false; }
    auto const& change = source.back();
    auto next = text_;
    next.replace(change.at, backward ? change.inserted.size() : change.removed.size(),
                 backward ? change.removed : change.inserted);
    auto breaks = grapheme_boundaries(next);
    auto const caret = backward ? change.before_caret : change.after_caret;
    auto const anchor = backward ? change.before_anchor : change.after_caret;
    destination.push_back(std::move(source.back()));
    source.pop_back();
    text_.swap(next);
    boundaries_.swap(breaks);
    caret_ = caret;
    anchor_ = anchor;
    preferred_column_.reset();
    return true;
}
auto TextEdit::undo() -> bool { return replay(true); }
auto TextEdit::redo() -> bool { return replay(false); }

auto TextEdit::paste(esc::PasteChunk const& chunk,
                     std::function<bool(char32_t)> const& accept) -> bool
{
    if (chunk.first) {
        paste_.clear();
        pasting_ = true;
    }
    if (!pasting_) { return false; }
    if (chunk.aborted) {
        paste_.clear();
        pasting_ = false;
        return false;
    }
    paste_ += chunk.bytes;
    if (!chunk.last) { return false; }
    pasting_ = false;
    auto bytes = std::exchange(paste_, {});
    return insert(bytes, accept);
}

auto TextEdit::key_press(KeyEvent const& key,
                         TextLayout const& layout,
                         std::function<bool(char32_t)> const& accept) -> EventResponse
{
    if (key.action == KeyAction::Release) { return {}; }
    auto matches = [&](auto code, Modifiers modifiers = {}) {
        return key.matches(code, modifiers, key.action);
    };
    auto changed = false;
    auto const shift = key.modifiers.shift;
    auto const ctrl = key.modifiers.ctrl;
    auto const modifiers = Modifiers{.shift = shift, .ctrl = ctrl};
    if (matches(U'a', {.ctrl = true})) { changed = select_all(); }
    else if (matches(U'z', {.ctrl = true})) {
        changed = undo();
    }
    else if (matches(U'y', {.ctrl = true}) ||
             matches(U'z', {.shift = true, .ctrl = true})) {
        changed = redo();
    }
    else if (matches(KeyCode::ArrowLeft, modifiers)) {
        changed = move_left(shift, ctrl);
    }
    else if (matches(KeyCode::ArrowRight, modifiers)) {
        changed = move_right(shift, ctrl);
    }
    else if (matches(KeyCode::Backspace, {.ctrl = ctrl})) {
        changed = erase_backward(ctrl);
    }
    else if (matches(KeyCode::Delete, {.ctrl = ctrl})) {
        changed = erase_forward(ctrl);
    }
    else if (matches(KeyCode::Home, modifiers) || matches(KeyCode::End, modifiers)) {
        if (layout.text() != text_) {
            throw std::invalid_argument{"Stale text layout"};
        }
        auto const home = matches(KeyCode::Home, modifiers);
        auto const line = layout.lines()[(std::size_t)layout.position(caret_).y];
        changed = set_caret(
            ctrl ? (home ? 0 : text_.size()) : (home ? line.begin : line.end), shift);
    }
    else if (multiline_ && (matches(KeyCode::ArrowUp, {.shift = shift}) ||
                            matches(KeyCode::ArrowDown, {.shift = shift}))) {
        changed = move_vertical(
            layout, matches(KeyCode::ArrowUp, {.shift = shift}) ? -1 : 1, shift);
    }
    else if (multiline_ && matches(KeyCode::Enter)) {
        changed = insert("\n", accept);
    }
    else {
        auto const input = input_text(key);
        if (input.empty()) { return {}; }
        changed = insert(esc::utf32_to_utf8(input), accept);
    }
    return {.handled = true, .repaint = changed};
}
}  // namespace ox
