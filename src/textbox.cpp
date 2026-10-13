#include <ox/textbox.hpp>

#include <algorithm>
#include <cassert>
#include <numeric>
#include <optional>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <zzz/coro.hpp>

#include <esc/text.hpp>

#include <ox/align.hpp>
#include <ox/core/core.hpp>
#include <ox/put.hpp>
#include <ox/scrollbar.hpp>
#include <ox/widget.hpp>

namespace {
using namespace ox;

/**
 * Calculate the spans of text that fit within the given width and wrap policy.
 * @details Each entry in the returned vector is a span of Glyphs that fit within the
 * given width.
 *
 * @param glyphs The text to calculate spans for, the underlying text must remain valid
 * for the lifetime of the returned vector.
 * @param wrap The wrap policy to use.
 * @param width The hard limit to wrap text at.
 * @returns A vector of std::u32string_view, each representing a horizontal line. Vector
 * will always have at least one element. It points to the text in \p glyphs.
 */
[[nodiscard]] auto perform_text_layout(std::u32string_view glyphs,
                                       TextBox::Wrap wrap,
                                       std::size_t width)
    -> std::vector<std::u32string_view>
{
    if (width == 0 || glyphs.empty()) { return {std::u32string_view{}}; }

    auto spans = std::vector<std::u32string_view>{};
    auto remaining = glyphs;

    while (!remaining.empty()) {
        auto length = std::size_t{0};
        auto columns = std::size_t{0};
        auto word_end = std::size_t{0};
        for (auto cluster : esc::graphemes(remaining)) {
            if (cluster == U"\n" || cluster == U"\r\n") {
                length += cluster.size();
                break;
            }
            auto cells = esc::grapheme_width(cluster);
            if (!cells || *cells == 0) {
                throw std::invalid_argument{"TextBox requires printable text"};
            }
            if (*cells > width - columns) {
                if (length == 0) { length = cluster.size(); }
                else if (wrap == TextBox::Wrap::Word && word_end != 0) {
                    length = word_end;
                }
                break;
            }
            columns += *cells;
            length += cluster.size();
            if (cluster == U" ") { word_end = length; }
        }
        spans.push_back(remaining.substr(0, length));
        remaining.remove_prefix(length);
    }

    // Add empty line if newline is last char
    if (!glyphs.empty() && glyphs.back() == U'\n') { spans.push_back({}); }

    return spans;
}

/**
 * Find the position on screen where the \p index would be located. \p index can be one
 * past the end of the text. This does not account for scrolling, you'll need to
 * subtract the top display line from the y coordinate to get the actual screen
 * position.
 */
[[nodiscard]] auto index_to_point(std::size_t index,
                                  std::ranges::random_access_range auto&& line_lengths)
    -> Point
{
    assert(std::size(line_lengths) > 0);

    auto current_index = std::size_t{0};
    for (auto y = std::ptrdiff_t{0}; y < std::ssize(line_lengths); ++y) {
        // Check if index falls within this line
        auto const len = line_lengths[y];
        if (index >= current_index && index < (current_index + (std::size_t)len)) {
            return {.x = (int)(index - current_index), .y = (int)y};
        }
        current_index += (std::size_t)len;
    }
    return {
        .x = line_lengths[std::ssize(line_lengths) - 1],
        .y = (int)std::size(line_lengths) - 1,
    };
}

/**
 * Find the nearest index into unicode_str_ (via \p line_lengths) to the given Point.
 * @details The Point should have the scroll_offset_ already applied, this works with
 * the 'abstract' representation before a window into it is applied.
 */
[[nodiscard]] auto point_to_nearest_index(
    Point p,
    std::ranges::random_access_range auto&& line_lengths) -> std::size_t
{
    p.y = std::max(p.y, 0);
    auto truncated = line_lengths | std::views::take(p.y);
    auto index = std::accumulate(std::begin(truncated), std::end(truncated), 0);
    index += std::min(p.x, std::max((int)line_lengths[p.y] - 1, 0));
    return (std::size_t)index;
}

/**
 * Calculates the amount that scroll_offset_ should be modified by in order to make the
 * given cursor index appear visible on screen. Returns 0 if already on screen.
 */
template <std::ranges::range R>
[[nodiscard]] auto y_offset_to_cursor_index(std::size_t cursor_index,
                                            R&& line_lengths,
                                            int scroll_offset_,
                                            int screen_height) -> int
{
    auto const at = index_to_point(cursor_index, line_lengths);
    if (at.y >= scroll_offset_ && at.y < scroll_offset_ + screen_height) { return 0; }
    else if (at.y < scroll_offset_) {
        return at.y - scroll_offset_;
    }
    else {
        return at.y - (scroll_offset_ + screen_height - 1);
    }
}

/**
 * Return a std::view of the lengths of each line in the given \p spans.
 */
[[nodiscard]]
auto line_lengths(std::vector<std::u32string_view> const& spans)
{
    return spans |
           std::views::transform([](auto const& span) { return (int)std::size(span); });
}

}  // namespace

namespace ox {

TextBox::Options const TextBox::init = {};

TextBox::TextBox(Options x)
    : Widget{x.focus_policy, x.size_policy},
      text{std::move(x.text)},
      wrap{std::move(x.wrap)},
      align{std::move(x.align)},
      style{std::move(x.style)}
{
    this->update_layout_cache();  // updates unicode_str_ and text_layout_
}

void TextBox::set_scroll_offset(int position)
{
    scroll_offset_ = std::clamp(position, 0, this->get_scroll_length());

    // Adjust cursor if needed.
    auto at = index_to_point(cursor_index_, line_lengths(text_layout_));
    if (at.y < scroll_offset_) {
        at.y = position;
        cursor_index_ = point_to_nearest_index(at, line_lengths(text_layout_));
    }
    else if (at.y >= scroll_offset_ + size.height) {
        at.y = position + std::max(size.height - 1, 0);
        cursor_index_ = point_to_nearest_index(at, line_lengths(text_layout_));
    }
}

auto TextBox::get_scroll_length() const -> int { return (int)std::ssize(text_layout_); }

void TextBox::paint(Canvas c)
{
    this->update_layout_cache();  // updates unicode_str_ and text_layout_

    {  // Adjust scroll and cursor if no longer valid.
        auto const line_count = (int)std::ssize(text_layout_);
        scroll_offset_ = std::clamp(scroll_offset_, 0, std::max(line_count - 1, 0));
        cursor_index_ =
            std::clamp(cursor_index_, std::size_t{0}, std::size(unicode_str_));
        scroll_offset_ += y_offset_to_cursor_index(
            cursor_index_, line_lengths(text_layout_), scroll_offset_, size.height);

        // TODO Always emitted, but could be updated to be smarter about this.
        this->on_scroll(scroll_offset_, line_count);
    }

    fill(c, style);

    // Paint Text
    for (auto y = 0;
         y + scroll_offset_ < std::ssize(text_layout_) && y < c.size().height; ++y) {
        auto span = text_layout_[(std::size_t)(y + scroll_offset_)];
        if (span.ends_with(U"\n")) { span.remove_suffix(1); }
        if (span.ends_with(U"\r")) { span.remove_suffix(1); }
        auto const width = esc::cell_width(span, c.width_policy());
        if (!width) { throw std::invalid_argument{"TextBox requires printable text"}; }
        auto const x = find_align_offset(align, c.size().width, (int)*width);
        put(c, {x, y}, esc::glyphs(span, style));
    }

    {  // Position the Cursor
        auto const cursor = index_to_point(cursor_index_, line_lengths(text_layout_));

        auto span = text_layout_[(std::size_t)cursor.y];
        if (span.ends_with(U"\n")) { span.remove_suffix(1); }
        if (span.ends_with(U"\r")) { span.remove_suffix(1); }
        auto const width = esc::cell_width(span, c.width_policy());
        if (!width) { throw std::invalid_argument{"TextBox requires printable text"}; }
        auto columns = std::size_t{0};
        auto offset = std::size_t{0};
        for (auto cluster : esc::graphemes(span)) {
            if (offset + cluster.size() > (std::size_t)cursor.x) { break; }
            columns += *esc::grapheme_width(cluster, c.width_policy());
            offset += cluster.size();
        }
        auto const x =
            find_align_offset(align, c.size().width, (int)*width) + (int)columns;
        Widget::cursor = Point{x, cursor.y - scroll_offset_};
    }
}

void TextBox::key_press(KeyEvent k)
{
    this->update_layout_cache();
    cursor_index_ = std::min(cursor_index_, unicode_str_.size());
    if (k.matches(KeyCode::Backspace, {}, k.action)) {
        if (!text.empty() && cursor_index_ > 0) {
            --cursor_index_;
            unicode_str_.erase(
                std::next(std::cbegin(unicode_str_), (std::ptrdiff_t)cursor_index_));
            text = esc::utf32_to_utf8(unicode_str_);
        }
    }
    else if (k.matches(KeyCode::Enter, {}, k.action)) {
        unicode_str_.insert(
            std::next(std::cbegin(unicode_str_), (std::ptrdiff_t)cursor_index_++),
            U'\n');
        text = esc::utf32_to_utf8(unicode_str_);
    }
    else if (k.matches(KeyCode::Delete, {}, k.action)) {
        if (cursor_index_ < unicode_str_.size()) {
            unicode_str_.erase(
                std::next(std::cbegin(unicode_str_), (std::ptrdiff_t)cursor_index_));
            text = esc::utf32_to_utf8(unicode_str_);
        }
    }
    else if (k.matches(KeyCode::ArrowLeft, {}, k.action)) {
        if (cursor_index_ > 0) { --cursor_index_; }
    }
    else if (k.matches(KeyCode::ArrowRight, {}, k.action)) {
        if (cursor_index_ < unicode_str_.size()) { ++cursor_index_; }
    }
    else if (k.matches(KeyCode::ArrowDown, {}, k.action)) {
        auto at = index_to_point(cursor_index_, line_lengths(text_layout_));
        at.y += 1;
        if (at.y < std::ssize(text_layout_)) {
            cursor_index_ = point_to_nearest_index(at, line_lengths(text_layout_));
        }
    }
    else if (k.matches(KeyCode::ArrowUp, {}, k.action)) {
        auto at = index_to_point(cursor_index_, line_lengths(text_layout_));
        at.y = std::max(at.y - 1, 0);
        cursor_index_ = point_to_nearest_index(at, line_lengths(text_layout_));
    }
    else if (k.matches(KeyCode::Home, {}, k.action)) {
        cursor_index_ = 0;
    }
    else if (k.matches(KeyCode::End, {}, k.action)) {
        cursor_index_ = unicode_str_.size();
    }
    else {
        auto const input = input_text(k);
        unicode_str_.insert(cursor_index_, input);
        cursor_index_ += input.size();
        text = esc::utf32_to_utf8(unicode_str_);
    }
}

void TextBox::paste(esc::PasteChunk const& chunk)
{
    if (chunk.first) { paste_buffer_.clear(); }
    paste_buffer_ += chunk.bytes;
    if (chunk.last) {
        if (!chunk.aborted) {
            unicode_str_ = esc::utf8_to_utf32(text);
            cursor_index_ = std::min(cursor_index_, unicode_str_.size());
            auto const input = esc::utf8_to_utf32(paste_buffer_);
            unicode_str_.insert(cursor_index_, input);
            cursor_index_ += input.size();
            text = esc::utf32_to_utf8(unicode_str_);
        }
        paste_buffer_.clear();
    }
}

void TextBox::mouse_press(MousePress m)
{
    if (m.button == MouseButton::Left) {
        // Normalize the mouse position
        auto const align_offset = [&]() -> int {
            assert(text_layout_.size() > 0);
            auto const span = text_layout_[(std::size_t)(std::min(
                scroll_offset_ + m.at.y, (int)text_layout_.size() - 1))];
            return find_align_offset(align, size.width, (int)span.size());
        }();

        m.at.x -= align_offset;
        m.at.y += scroll_offset_;
        cursor_index_ = point_to_nearest_index(m.at, line_lengths(text_layout_));
    }
}

void TextBox::mouse_wheel(MouseWheel m)
{
    if (m.dy != 0) { this->set_scroll_offset(scroll_offset_ + m.dy); }
}

void TextBox::update_layout_cache()
{
    unicode_str_ = esc::utf8_to_utf32(text);
    text_layout_ = perform_text_layout(unicode_str_, wrap, (std::size_t)size.width);
}

void link(TextBox& tb, ScrollBar& sb)
{
    Connection{
        .signal = tb.on_scroll,
        .slot =
            [](int pos, int len, ScrollBar& sb) {
                sb.position = pos;
                sb.scrollable_length = len;
            },
    }(sb);

    Connection{
        .signal = sb.on_scroll,
        .slot = [](int pos, TextBox& tb) { tb.set_scroll_offset(pos); },
    }(tb);
}

}  // namespace ox
