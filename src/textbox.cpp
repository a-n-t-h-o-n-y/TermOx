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

#include <esc/detail/transcode.hpp>

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
        auto line = remaining.substr(0, width);
        auto const newline_at = line.find_first_of(U'\n');
        if (newline_at != std::u32string_view::npos) {
            line = line.substr(0, newline_at + 1);
        }

        if (wrap == TextBox::Wrap::Word && line.size() == width) {
            auto const last_space = line.find_last_of(U' ');
            if (last_space != std::u32string_view::npos) {
                line = line.substr(0, last_space + 1);
            }
        }
        spans.push_back(line);
        remaining = remaining.substr(line.size());
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
      brush{std::move(x.brush)}
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

    fill(c, brush);

    // Paint Text
    for (auto y = 0; y + scroll_offset_ < std::ssize(text_layout_) && y < c.size.height;
         ++y) {
        auto const span = text_layout_[(std::size_t)(y + scroll_offset_)];
        auto const x = find_align_offset(align, c.size.width, (int)span.size());

        for (auto i = 0; i < (int)span.size(); ++i) {
            auto ch = span[(std::size_t)i];
            if (ch == U'\n') { continue; }
            c[{.x = x + i, .y = y}].symbol = ch;
        }
    }

    {  // Position the Cursor
        auto const cursor = index_to_point(cursor_index_, line_lengths(text_layout_));

        auto const span = text_layout_[(std::size_t)cursor.y];
        auto const x =
            find_align_offset(align, c.size.width, (int)std::ssize(span)) + cursor.x;
        Widget::cursor = {.x = x, .y = cursor.y - scroll_offset_};
    }
}

void TextBox::key_press(Key k)
{
    switch (k) {
        case Key::Backspace:
            if (!text.empty() && cursor_index_ > 0) {
                --cursor_index_;
                unicode_str_.erase(std::next(std::cbegin(unicode_str_),
                                             (std::ptrdiff_t)cursor_index_));
                esc::detail::u32_to_u8(unicode_str_, text);
            }
            break;
        case Key::Enter:
            unicode_str_.insert(
                std::next(std::cbegin(unicode_str_), (std::ptrdiff_t)cursor_index_++),
                U'\n');
            esc::detail::u32_to_u8(unicode_str_, text);
            break;
        case Key::Delete:
            if (cursor_index_ < text.size()) {
                unicode_str_.erase(std::next(std::cbegin(unicode_str_),
                                             (std::ptrdiff_t)cursor_index_));
                esc::detail::u32_to_u8(unicode_str_, text);
            }
            break;
        case Key::ArrowLeft:
            if (cursor_index_ > 0) { --cursor_index_; }
            break;
        case Key::ArrowRight:
            if (cursor_index_ < unicode_str_.size()) { ++cursor_index_; }
            break;
        case Key::ArrowDown: {
            auto at = index_to_point(cursor_index_, line_lengths(text_layout_));
            at.y += 1;
            if (at.y >= std::ssize(text_layout_)) { break; }
            cursor_index_ = point_to_nearest_index(at, line_lengths(text_layout_));
            break;
        }
        case Key::ArrowUp: {
            auto at = index_to_point(cursor_index_, line_lengths(text_layout_));
            at.y = std::max(at.y - 1, 0);
            cursor_index_ = point_to_nearest_index(at, line_lengths(text_layout_));
            break;
        }
        case Key::Home: cursor_index_ = 0; break;
        case Key::End: cursor_index_ = unicode_str_.size(); break;
        default:
            if (auto const c = esc::key_to_char32(k); c != U'\0') {
                unicode_str_.insert(
                    std::next(std::begin(unicode_str_), (std::ptrdiff_t)cursor_index_),
                    c);
                esc::detail::u32_to_u8(unicode_str_, text);
                ++cursor_index_;
            }
            break;
    }
}

void TextBox::mouse_press(Mouse m)
{
    if (m.button == Mouse::Button::Left) {
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

void TextBox::mouse_wheel(Mouse m)
{
    if (m.button == Mouse::Button::ScrollUp) {
        this->set_scroll_offset(scroll_offset_ - 1);
    }
    else if (m.button == Mouse::Button::ScrollDown) {
        this->set_scroll_offset(scroll_offset_ + 1);
    }
}

void TextBox::update_layout_cache()
{
    esc::detail::u8_string_to_u32_string(text, unicode_str_);
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
