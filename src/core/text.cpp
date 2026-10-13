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

auto columns(std::string_view cluster, esc::WidthPolicy policy) -> int
{
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

namespace {
constexpr auto tab_columns = 0;
constexpr auto newline_columns = -1;

void validate_layout(int width, TextWrap wrap, int tabs)
{
    if (width < 0 || tabs <= 0 ||
        (wrap != TextWrap::None && wrap != TextWrap::Any && wrap != TextWrap::Word)) {
        throw std::invalid_argument{"Invalid text layout"};
    }
}

void analyze_text(std::string_view text, std::vector<std::size_t>& boundaries)
{
    boundaries.clear();
    boundaries.push_back(0);
    auto at = std::size_t{0};
    for (auto cluster : esc::graphemes(text)) {
        at += cluster.size();
        boundaries.push_back(at);
    }
    // Segmentation validates the complete UTF-8 input first. C0/DEL occupy one
    // byte; valid UTF-8 encodes C1 controls only as C2 80..9F. No decoded or
    // normalized string is needed, and original CR/CRLF byte offsets are retained.
    for (auto i = std::size_t{0}; i < text.size(); ++i) {
        auto const byte = (unsigned char)text[i];
        if ((byte < 32 && byte != '\n' && byte != '\r' && byte != '\t') ||
            byte == 127 || (byte == 0xc2 && (unsigned char)text[i + 1] <= 0x9f)) {
            throw std::invalid_argument{"Text contains a terminal control character"};
        }
    }
}

void measure_columns(std::string_view text,
                     std::span<std::size_t const> boundaries,
                     esc::WidthPolicy policy,
                     std::vector<int>& measured)
{
    if (esc::grapheme_width(" ", policy) != 1) {
        throw std::invalid_argument{"Text layout requires one-column spaces"};
    }
    measured.clear();
    measured.reserve(boundaries.size() - 1);
    for (auto i = std::size_t{0}; i + 1 < boundaries.size(); ++i) {
        auto const cluster =
            text.substr(boundaries[i], boundaries[i + 1] - boundaries[i]);
        measured.push_back(cluster == "\t" ? tab_columns
                           : cluster == "\n" || cluster == "\r" || cluster == "\r\n"
                               ? newline_columns
                               : columns(cluster, policy));
    }
}

void wrap_rows(std::string_view text,
               std::span<std::size_t const> boundaries,
               std::span<int const> measured,
               int width,
               TextWrap wrap,
               int tabs,
               std::vector<TextLine>& lines,
               std::vector<Point>* positions = nullptr)
{
    lines.clear();
    if (positions) { positions->resize(boundaries.size()); }
    auto record = [&](std::size_t at, int x) {
        if (lines.size() >= (std::size_t)std::numeric_limits<int>::max()) {
            throw std::length_error{"Too many text rows"};
        }
        if (positions) { (*positions)[at] = {x, (int)lines.size()}; }
    };
    auto begin = std::size_t{0};
    while (begin < measured.size()) {
        auto end = begin;
        auto next = begin;
        auto used = 0;
        auto word_end = begin;
        auto word_columns = 0;
        while (end < measured.size()) {
            if (measured[end] == newline_columns) {
                next = end + 1;
                break;
            }
            auto const cells =
                measured[end] == tab_columns ? tabs - used % tabs : measured[end];
            if (wrap != TextWrap::None && width > 0 &&
                cells > width - std::min(used, width) && end != begin) {
                if (wrap == TextWrap::Word && word_end > begin) {
                    end = word_end;
                    used = word_columns;
                }
                next = end;
                break;
            }
            record(end, used);
            used = add_columns(used, cells);
            ++end;
            next = end;
            if (boundaries[end] - boundaries[end - 1] == 1 &&
                (text[boundaries[end - 1]] == ' ' ||
                 text[boundaries[end - 1]] == '\t')) {
                word_end = end;
                word_columns = used;
            }
        }
        record(end, used);
        lines.push_back({boundaries[begin], boundaries[end], boundaries[next], used});
        begin = next;
    }
    if (lines.empty() || lines.back().next > lines.back().end ||
        (wrap != TextWrap::None && width > 0 && lines.back().columns >= width)) {
        record(measured.size(), 0);
        lines.push_back({text.size(), text.size(), text.size(), 0});
    }
}
}  // namespace

auto layout_text(std::string_view text,
                 int width,
                 TextWrap wrap,
                 esc::WidthPolicy policy,
                 int tab_width) -> std::vector<TextLine>
{
    validate_layout(width, wrap, tab_width);
    auto boundaries = std::vector<std::size_t>{};
    analyze_text(text, boundaries);
    auto measured = std::vector<int>{};
    measure_columns(text, boundaries, policy, measured);
    auto lines = std::vector<TextLine>{};
    wrap_rows(text, boundaries, measured, width, wrap, tab_width, lines);
    return lines;
}

void swap(TextLayout& a, TextLayout& b) noexcept
{
    using std::swap;
    swap(a.text_, b.text_);
    swap(a.lines_, b.lines_);
    swap(a.boundaries_, b.boundaries_);
    swap(a.positions_, b.positions_);
    swap(a.columns_, b.columns_);
    swap(a.text_storage_, b.text_storage_);
    swap(a.line_storage_, b.line_storage_);
    swap(a.boundary_storage_, b.boundary_storage_);
    swap(a.position_storage_, b.position_storage_);
    swap(a.column_storage_, b.column_storage_);
    swap(a.override_storage_, b.override_storage_);
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
    auto const new_text = width_ < 0 || text_ != text;
    auto const new_policy = width_ < 0 || !same_policy(this->policy(), policy);
    if (!new_text && !new_policy && width_ == width && wrap_ == wrap &&
        tab_width_ == tab_width) {
        return false;
    }
    validate_layout(width, wrap, tab_width);
    if (new_text) {
        analyze_text(text, boundary_storage_);
        text_storage_.assign(text);
    }
    auto const& boundaries = new_text ? boundary_storage_ : boundaries_;
    if (new_text || new_policy) {
        measure_columns(text, boundaries, policy, column_storage_);
    }
    auto const& measured = new_text || new_policy ? column_storage_ : columns_;
    wrap_rows(text, boundaries, measured, width, wrap, tab_width, line_storage_,
              &position_storage_);
    if (new_policy) {
        override_storage_.assign(policy.overrides.begin(), policy.overrides.end());
    }

    if (new_text) {
        text_.swap(text_storage_);
        boundaries_.swap(boundary_storage_);
    }
    if (new_text || new_policy) { columns_.swap(column_storage_); }
    lines_.swap(line_storage_);
    positions_.swap(position_storage_);
    if (new_policy) {
        overrides_.swap(override_storage_);
        policy_ = policy;
        policy_.overrides = {};
    }
    width_ = width;
    wrap_ = wrap;
    tab_width_ = tab_width;
    text_storage_.clear();
    boundary_storage_.clear();
    column_storage_.clear();
    line_storage_.clear();
    position_storage_.clear();
    override_storage_.clear();
    return true;
}

auto TextLayout::position(std::size_t offset) const -> Point
{
    auto const found = std::ranges::lower_bound(boundaries_, offset);
    if (found == boundaries_.end() || *found != offset) {
        throw std::invalid_argument{"Caret is not a grapheme boundary"};
    }
    return positions_[(std::size_t)(found - boundaries_.begin())];
}

auto TextLayout::offset(Point point) const -> std::size_t
{
    auto const y = std::clamp(point.y, 0, (int)lines_.size() - 1);
    auto const& line = lines_[(std::size_t)y];
    if (point.x <= 0) { return line.begin; }
    if (point.x >= line.columns) { return line.end; }
    auto const begin = (std::size_t)(std::ranges::lower_bound(boundaries_, line.begin) -
                                     boundaries_.begin());
    auto const end = (std::size_t)(std::ranges::lower_bound(boundaries_, line.end) -
                                   boundaries_.begin());
    auto const found =
        std::upper_bound(positions_.begin() + (std::ptrdiff_t)begin,
                         positions_.begin() + (std::ptrdiff_t)end, point.x,
                         [](int x, Point at) { return x < at.x; });
    return boundaries_[(std::size_t)(found - positions_.begin()) - 1];
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
    swap(a.text_storage_, b.text_storage_);
    swap(a.boundary_storage_, b.boundary_storage_);
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
    text_storage_.clear();
    boundary_storage_.clear();
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

void TextEdit::prepare_replace(std::size_t begin,
                               std::size_t end,
                               std::string_view value)
{
    text_storage_.assign(text_);
    text_storage_.replace(begin, end - begin, value);

    // LF always breaks graphemes and resets Unicode segmentation context. Edits
    // can join clusters across removed newlines, so include the entire affected
    // logical lines through the next surviving LF, not a fixed-size neighborhood.
    auto const previous = begin == 0 ? std::string::npos : text_.rfind('\n', begin - 1);
    auto const start = previous == std::string::npos ? 0 : previous + 1;
    auto const newline = text_.find('\n', end);
    auto const stop = newline == std::string::npos ? text_.size() : newline + 1;
    auto const new_stop = stop - (end - begin) + value.size();
    auto const prefix = std::ranges::upper_bound(boundaries_, start);
    auto const suffix = std::ranges::upper_bound(boundaries_, stop);
    boundary_storage_.reserve(boundaries_.size());
    boundary_storage_.assign(boundaries_.begin(), prefix);
    auto at = start;
    for (auto cluster : esc::graphemes(
             std::string_view{text_storage_}.substr(start, new_stop - start))) {
        at += cluster.size();
        boundary_storage_.push_back(at);
    }
    auto const tail = boundary_storage_.size();
    boundary_storage_.resize(tail + (std::size_t)(boundaries_.end() - suffix));
    std::transform(
        suffix, boundaries_.end(), boundary_storage_.begin() + (std::ptrdiff_t)tail,
        [&](std::size_t offset) { return offset - (end - begin) + value.size(); });
}

void TextEdit::commit_text() noexcept
{
    text_.swap(text_storage_);
    boundaries_.swap(boundary_storage_);
    text_storage_.clear();
    boundary_storage_.clear();
}

auto TextEdit::replace(std::size_t begin, std::size_t end, std::string value) -> bool
{
    auto const same_text = std::string_view{text_}.substr(begin, end - begin) == value;
    if (same_text && anchor_ == end && caret_ == end) { return false; }
    auto after = end;
    if (!same_text) {
        prepare_replace(begin, end, value);
        after = *std::ranges::lower_bound(boundary_storage_, begin + value.size());
    }
    if (history_limit_ > 0) {
        undo_.push_back({begin, text_.substr(begin, end - begin), std::move(value),
                         anchor_, caret_, after});
    }
    if (!same_text) { commit_text(); }
    anchor_ = caret_ = after;
    preferred_column_.reset();
    redo_.clear();
    if (undo_.size() > history_limit_) { undo_.pop_front(); }
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
    auto const same_text = change.removed == change.inserted;
    if (!same_text) {
        prepare_replace(
            change.at,
            change.at + (backward ? change.inserted.size() : change.removed.size()),
            backward ? change.removed : change.inserted);
    }
    auto const caret = backward ? change.before_caret : change.after_caret;
    auto const anchor = backward ? change.before_anchor : change.after_caret;
    destination.push_back(std::move(source.back()));
    source.pop_back();
    if (!same_text) { commit_text(); }
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
