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
    assert(line_lengths.size() > 0);

    auto current_index = 0;
    for (auto y = 0; y < std::ssize(line_lengths); ++y) {
        // Check if index falls within this line
        auto const len = line_lengths[(std::size_t)y];
        if ((int)index >= current_index && (int)index < current_index + len) {
            return {.x = (int)index - current_index, .y = y};
        }
        current_index += (int)len;
    }
    return {
        .x = line_lengths[std::size(line_lengths) - 1],
        .y = (int)std::ssize(line_lengths) - 1,
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
    index += std::min(p.x, std::max((int)line_lengths[(std::size_t)p.y] - 1, 0));
    return index;
}

/**
 * Calculates the amount that scroll_offset should be modified by in order to make the
 * given cursor index appear visible on screen. Returns 0 if already on screen.
 */
template <std::ranges::range R>
[[nodiscard]] auto y_offset_to_cursor_index(std::size_t cursor_index,
                                            R&& line_lengths,
                                            int scroll_offset,
                                            int screen_height) -> int
{
    auto const at = index_to_point(cursor_index, line_lengths);
    if (at.y >= scroll_offset && at.y < scroll_offset + screen_height) { return 0; }
    else if (at.y < scroll_offset) {
        return at.y - scroll_offset;
    }
    else {
        return at.y - (scroll_offset + screen_height - 1);
    }
}

/**
 * Return a std::view of the lengths of each line in the given \p spans.
 */
[[nodiscard]]
auto line_lengths(std::vector<std::u32string_view> const& spans)
{
    return spans |
           std::views::transform([](auto const& span) { return std::ssize(span); });
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
{}

void TextBox::paint(Canvas c)
{
    this->update_layout_cache();  // updates unicode_str_ and text_layout_

    {  // Adjust scroll and cursor if no longer valid.
        auto const line_count = (int)std::ssize(text_layout_);
        scroll_offset = std::clamp(scroll_offset, 0, std::max(line_count - 1, 0));
        cursor_index_ =
            std::clamp(cursor_index_, std::size_t{0}, std::size(unicode_str_));
        scroll_offset += y_offset_to_cursor_index(
            cursor_index_, line_lengths(text_layout_), scroll_offset, size.height);

        // TODO Always emitted, but could be updated to be smarter about this.
        this->on_scroll(scroll_offset, line_count);
    }

    // Fill with Brush
    for (auto x = 0; x < c.size.width; ++x) {
        for (auto y = 0; y < c.size.height; ++y) {
            c[{.x = x, .y = y}].brush = brush;
        }
    }

    // Paint Text
    for (auto y = 0; y + scroll_offset < std::ssize(text_layout_) && y < c.size.height;
         ++y) {
        auto const span = text_layout_[(std::size_t)(y + scroll_offset)];
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
        Widget::cursor = {.x = x, .y = cursor.y - scroll_offset};
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
                text = esc::detail::u32_to_u8(unicode_str_);
            }
            break;
        case Key::Enter:
            unicode_str_.insert(
                std::next(std::cbegin(unicode_str_), (std::ptrdiff_t)cursor_index_++),
                U'\n');
            text = esc::detail::u32_to_u8(unicode_str_);
            break;
        case Key::Delete:
            if (cursor_index_ < text.size()) {
                unicode_str_.erase(std::next(std::cbegin(unicode_str_),
                                             (std::ptrdiff_t)cursor_index_));
                text = esc::detail::u32_to_u8(unicode_str_);
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
                text = esc::detail::u32_to_u8(unicode_str_);
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
                scroll_offset + m.at.y, (int)text_layout_.size() - 1))];
            return find_align_offset(align, size.width, (int)span.size());
        }();

        m.at.x -= align_offset;
        m.at.y += scroll_offset;
        cursor_index_ = point_to_nearest_index(m.at, line_lengths(text_layout_));
    }
}

void TextBox::mouse_wheel(Mouse m)
{
    if (m.button == Mouse::Button::ScrollUp) {
        if (scroll_offset > 0) {
            --scroll_offset;
            auto at = index_to_point(cursor_index_, line_lengths(text_layout_));
            if (at.y >= scroll_offset + size.height) {
                at.y = std::max(at.y - 1, 0);
                cursor_index_ = point_to_nearest_index(at, line_lengths(text_layout_));
            }
        }
    }
    else if (m.button == Mouse::Button::ScrollDown) {
        if (scroll_offset + 1 < std::ssize(text_layout_)) {
            ++scroll_offset;
            auto at = index_to_point(cursor_index_, line_lengths(text_layout_));
            if (at.y < scroll_offset) {
                at.y += 1;
                cursor_index_ = point_to_nearest_index(at, line_lengths(text_layout_));
            }
        }
    }
}

void TextBox::update_layout_cache()
{
    unicode_str_ = [this] {
        auto gen = esc::detail::u8_string_to_u32(text);
        return std::u32string{std::begin(gen), std::end(gen)};
    }();
    text_layout_ = perform_text_layout(unicode_str_, wrap, (std::size_t)size.width);
}

void link(TextBox& tb, ScrollBar& sb)
{
    tb.on_scroll.connect(tracked(
        [](ScrollBar& sb, int pos, int len) {
            sb.position = pos;
            sb.scrollable_length = len;
        },
        sb));
    sb.on_scroll.connect(
        tracked([](TextBox& tb, int pos) { tb.scroll_offset = pos; }, tb));
}

}  // namespace ox
