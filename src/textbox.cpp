#include <ox/textbox.hpp>

#include <algorithm>
#include <cassert>
#include <optional>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>

#include <ox/core/core.hpp>
#include <ox/scrollbar.hpp>
#include <ox/widget.hpp>

namespace {
using namespace ox;

/**
 * Calculate the spans of text that fit within the given width and wrap policy.
 *
 * @details Each entry in the returned vector is a span of Glyphs that fit within the
 * given width.
 * @param glyphs The text to calculate spans for.
 * @param wrap The wrap policy to use.
 * @param width The hard limit to wrap text at.
 * @returns A vector of spans, each representing a horizontal line. Vector will always
 * have at least one element.
 */
[[nodiscard]] auto calculate_spans(std::vector<Glyph> const& glyphs,
                                   TextBox::Wrap wrap,
                                   std::size_t width)
    -> std::vector<std::span<Glyph const>>
{
    constexpr auto is_space = [](Glyph const& g) {
        return std::u32string_view{U" \t\n\r\f\v"}.find(g.symbol) !=
               std::u32string_view::npos;
    };

    if (width == 0 || glyphs.empty()) {
        return {std::span<Glyph const>{}};
    }

    auto spans = std::vector<std::span<Glyph const>>{};
    auto full_span = std::span<Glyph const>{glyphs};

    while (!full_span.empty()) {
        auto const max_count = std::min(std::size(full_span), width);

        auto const count_to_newline =
            (std::size_t)std::distance(
                std::cbegin(full_span),
                std::ranges::find(full_span, U'\n', &Glyph::symbol)) +
            1;

        auto split_count = std::min(count_to_newline, max_count);

        // If word-wrapping, search for space char nearest the end of the line.
        if (wrap == TextBox::Wrap::Word && split_count == width) {
            auto const last_space_iter = [&] {
                auto const reverse_split_iter =
                    std::next(std::rbegin(full_span),
                              (std::ptrdiff_t)(std::size(full_span) - width));
                return std::find_if(reverse_split_iter, std::rend(full_span), is_space);
            }();

            auto const distance_to_last_space =
                (std::size_t)std::distance(last_space_iter, std::rend(full_span));

            if (distance_to_last_space > 0) {
                split_count = distance_to_last_space;
            }
        }

        spans.push_back(full_span.subspan(0, split_count));
        full_span = full_span.subspan(split_count);
    }

    // Add empty line if newline is last char
    if (!glyphs.empty() && glyphs.back().symbol == U'\n') {
        spans.push_back({});
    }

    return spans;
}

/**
 * Find the position on screen where the \p index would be located. \p index can be one
 * past the end of the text. This does not account for scrolling, you'll need to
 * subtract the top display line from the y coordinate to get the actual screen
 * position.
 */
template <std::ranges::range R>
[[nodiscard]] auto index_to_screen(std::size_t index, R&& line_lengths) -> Point
{
    auto current_line = 0;
    auto current_pos = 0;
    auto last_len = 0;  // For one past the last index

    for (auto const len : line_lengths) {
        // Check if index falls within this line
        if ((int)index >= current_pos && (int)index < current_pos + len) {
            return {.x = (int)index - current_pos, .y = current_line};
        }
        current_pos += (int)len;
        ++current_line;
        last_len = (int)len;
    }
    return {.x = last_len, .y = current_line - 1};
}

/**
 * Convert a screen position to a text index. The input point should have top_line_
 * added into the y coordinate, this does not consider scroll, the 'screen' is the
 * entire text display.
 */
template <std::ranges::range R>
[[nodiscard]] auto screen_to_index(Point p, R&& line_lengths) -> std::size_t
{
    auto const line_count = (int)std::ssize(line_lengths);

    // Past the end of text
    if (p.y >= line_count) {
        p.y = line_count - 1;
        p.x = (int)line_lengths[p.y];
    }

    auto index = std::size_t{0};
    for (auto i = 0; i < p.y; ++i) {
        index += (std::size_t)line_lengths[i];
    }
    auto const adjustment = (p.y + 1 == line_count) ? 0 : -1;
    return index + (std::size_t)std::min(p.x, (int)line_lengths[p.y] + adjustment);
}

/**
 * Calculates the amount that top_line should be offset by in order to make the given
 * cursor index appear visible on screen. Returns 0 if already on screen.
 * @details top_line is assumed to be less than line_lengths size.
 */
template <std::ranges::range R>
[[nodiscard]] auto scroll_offset_to_cursor_index(std::size_t cursor_index,
                                                 R&& line_lengths,
                                                 int top_line,
                                                 int screen_height) -> int
{
    auto current_index = std::size_t{0};
    auto const visible_end = top_line + screen_height - 1;

    // Find target line
    int target_line = 0;
    for (auto const len : line_lengths) {
        if (cursor_index < current_index + (std::size_t)len) {
            break;
        }
        current_index += (std::size_t)len;
        ++target_line;
    }

    // Return 0 if visible, otherwise calculate scroll offset
    if (target_line >= top_line && target_line <= visible_end) {
        return 0;
    }
    return (target_line < top_line) ? target_line - top_line
                                    : target_line - visible_end;
}

/**
 * Returns the cursor index that is nearest to the given \p cursor_index and is on
 * screen. Returns \p cursor_index if it is already on screen.
 */
template <std::ranges::range R>
[[nodiscard]] auto nearest_on_screen_cursor_index(std::size_t cursor_index,
                                                  R&& line_lengths,
                                                  int top_line,
                                                  int screen_height) -> std::size_t
{
    auto current_index = std::size_t{0};
    auto screen_start_index = std::size_t{0};
    auto screen_end_index = std::size_t{0};
    auto current_line = 0;

    for (auto const len : line_lengths) {
        // Track the start position of visible area
        if (current_line == top_line) {
            screen_start_index = current_index;
        }

        // Track the end position of visible area
        if (current_line == top_line + screen_height - 1) {
            screen_end_index = current_index + (std::size_t)len;
            break;
        }

        current_index += (std::size_t)len;
        ++current_line;
    }

    if (screen_end_index == 0) {
        screen_end_index = current_index;
    }

    // Cursor is already visible
    if (cursor_index >= screen_start_index && cursor_index < screen_end_index) {
        return cursor_index;
    }

    // Cursor is above
    if (cursor_index < screen_start_index) {
        return screen_start_index;
    }

    // Cursor is below
    return screen_end_index > 0 ? screen_end_index - 1 : 0;
}

[[nodiscard]] auto line_lengths(std::vector<std::span<Glyph const>> const& spans)
{
    return spans | std::ranges::views::transform(
                       [](auto const& span) { return std::ssize(span); });
}

}  // namespace

namespace ox {

TextBox::Options const TextBox::init = {};

TextBox::TextBox(Options x)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      text_{std::move(x.text)},
      wrap_{std::move(x.wrap)},
      align_{std::move(x.align)},
      background_{std::move(x.background)},
      insert_brush_{std::move(x.insert_brush)},
      spans_{calculate_spans(text_, wrap_, (std::size_t)size.width)}
{}

void TextBox::set_text(std::vector<Glyph> text)
{
    text_ = std::move(text);
    cursor_index_ = std::min(cursor_index_, text_.size());
    spans_ = calculate_spans(text_, wrap_, (std::size_t)size.width);
    top_line_ = std::min(top_line_, (int)std::size(spans_) - 1);
}

auto TextBox::get_text() const -> std::vector<Glyph> const& { return text_; }

void TextBox::set_wrap(Wrap wrap)
{
    wrap_ = wrap;
    spans_ = calculate_spans(text_, wrap_, (std::size_t)size.width);
    top_line_ += scroll_offset_to_cursor_index(cursor_index_, line_lengths(spans_),
                                               top_line_, size.height);
}

auto TextBox::get_wrap() const -> Wrap { return wrap_; }

void TextBox::set_align(Align align) { align_ = align; }

auto TextBox::get_align() const -> Align { return align_; }

void TextBox::set_background(Color color) { background_ = color; }

auto TextBox::get_background() const -> Color { return background_; }

void TextBox::set_insert_brush(Brush brush) { insert_brush_ = brush; }

auto TextBox::get_insert_brush() const -> Brush { return insert_brush_; }

void TextBox::set_top_line(int line)
{
    top_line_ = std::clamp(line, 0, (int)std::size(spans_) - 1);
    cursor_index_ = nearest_on_screen_cursor_index(cursor_index_, line_lengths(spans_),
                                                   top_line_, size.height);
}

void TextBox::paint(Canvas c)
{
    auto const align_offset = [this, &c](auto& span) -> int {
        switch (align_) {
            case Align::Left: return 0;
            case Align::Center: return (c.size.width - (int)span.size()) / 2;
            case Align::Right: return c.size.width - (int)span.size();
            default: throw std::logic_error{"Invalid Align"};
        }
    };

    Painter{c}.fill(U' ' | bg(background_));

    // Always emitted, but could be updated to be smarter about this.
    auto const line_count = (int)std::size(spans_);
    top_line_ = std::clamp(0, top_line_, std::max(line_count - 1, 0));
    this->on_scroll_update(top_line_, line_count);

    for (auto i = top_line_; i < std::ssize(spans_) && (i - top_line_) < c.size.height;
         ++i) {
        auto const span = spans_[(std::size_t)i];
        auto const x = align_offset(span);

        // Overwrite bg if Glyph's bg is XColor::Default.
        auto writer = Painter{c}[{x, i - top_line_}];
        for (Glyph g : span) {
            if (g.symbol == U'\n') {
                continue;
            }
            if (g.brush.background == Color{XColor::Default}) {
                g.brush.background = background_;
            }
            writer << g;
        }
    }

    auto const cursor = index_to_screen(cursor_index_, line_lengths(spans_));

    assert(cursor.y >= top_line_ && cursor.y < top_line_ + this->size.height);

    // Adjust x for Alignment
    auto const span = spans_[(std::size_t)cursor.y];
    auto const x = align_offset(span) + cursor.x;
    Widget::cursor = {.x = x, .y = cursor.y - top_line_};
}

void TextBox::key_press(Key k)
{
    switch (k) {
        case Key::Backspace:
            if (!text_.empty() && cursor_index_ > 0) {
                --cursor_index_;
                text_.erase(
                    std::next(std::cbegin(text_), (std::ptrdiff_t)cursor_index_));
                spans_ = calculate_spans(text_, wrap_, (std::size_t)size.width);
                top_line_ += scroll_offset_to_cursor_index(
                    cursor_index_, line_lengths(spans_), top_line_, size.height);
            }
            break;
        case Key::Enter:
            text_.insert(std::next(std::cbegin(text_), (std::ptrdiff_t)cursor_index_++),
                         {U'\n'});
            spans_ = calculate_spans(text_, wrap_, (std::size_t)size.width);
            top_line_ += scroll_offset_to_cursor_index(
                cursor_index_, line_lengths(spans_), top_line_, size.height);
            break;
        case Key::Delete:
            if (cursor_index_ < text_.size()) {
                text_.erase(
                    std::next(std::cbegin(text_), (std::ptrdiff_t)cursor_index_));
                spans_ = calculate_spans(text_, wrap_, (std::size_t)size.width);
                top_line_ += scroll_offset_to_cursor_index(
                    cursor_index_, line_lengths(spans_), top_line_, size.height);
            }
            break;
        case Key::ArrowLeft:
            if (cursor_index_ > 0) {
                --cursor_index_;
                top_line_ += scroll_offset_to_cursor_index(
                    cursor_index_, line_lengths(spans_), top_line_, size.height);
            }
            break;
        case Key::ArrowRight:
            if (cursor_index_ < text_.size()) {
                ++cursor_index_;
                top_line_ += scroll_offset_to_cursor_index(
                    cursor_index_, line_lengths(spans_), top_line_, size.height);
            }
            break;
        case Key::ArrowDown: {
            auto at = index_to_screen(cursor_index_, line_lengths(spans_));
            at.y += 1;
            cursor_index_ = screen_to_index(at, line_lengths(spans_));
            top_line_ += scroll_offset_to_cursor_index(
                cursor_index_, line_lengths(spans_), top_line_, size.height);
            break;
        }
        case Key::ArrowUp: {
            auto at = index_to_screen(cursor_index_, line_lengths(spans_));
            at.y = std::max(at.y - 1, 0);
            cursor_index_ = screen_to_index(at, line_lengths(spans_));
            top_line_ += scroll_offset_to_cursor_index(
                cursor_index_, line_lengths(spans_), top_line_, size.height);
            break;
        }
        case Key::Home:
            cursor_index_ = 0;
            top_line_ += scroll_offset_to_cursor_index(
                cursor_index_, line_lengths(spans_), top_line_, size.height);
            break;
        case Key::End:
            cursor_index_ = text_.size();
            top_line_ += scroll_offset_to_cursor_index(
                cursor_index_, line_lengths(spans_), top_line_, size.height);
            break;
        default:
            if (auto const c = esc::key_to_char(k); c != '\0') {
                text_.insert(
                    std::next(std::begin(text_), (std::ptrdiff_t)cursor_index_),
                    {
                        .symbol = (char32_t)c,
                        .brush = insert_brush_,
                    });
                ++cursor_index_;
                spans_ = calculate_spans(text_, wrap_, (std::size_t)size.width);
                top_line_ += scroll_offset_to_cursor_index(
                    cursor_index_, line_lengths(spans_), top_line_, size.height);
            }
            break;
    }
}

void TextBox::mouse_press(Mouse m)
{
    // Adjust the mouse position
    auto const align_offset = [&]() -> int {
        auto const span =
            spans_[(std::size_t)(std::min(top_line_ + m.at.y, (int)spans_.size() - 1))];
        switch (align_) {
            case Align::Left: return 0;
            case Align::Center: return (this->size.width - (int)span.size()) / 2;
            case Align::Right: return this->size.width - (int)span.size();
            default: throw std::logic_error{"Invalid Align"};
        }
    }();

    m.at.x -= align_offset;
    m.at.y += top_line_;
    cursor_index_ = screen_to_index(m.at, line_lengths(spans_));
}

void TextBox::mouse_wheel(Mouse m)
{
    if (m.button == Mouse::Button::ScrollUp) {
        if (top_line_ > 0) {
            this->set_top_line(top_line_ - 1);
        }
    }
    else if (m.button == Mouse::Button::ScrollDown) {
        this->set_top_line(std::min(top_line_ + 1, (int)std::size(spans_) - 1));
    }
}

void TextBox::resize(Area)
{
    spans_ = calculate_spans(text_, wrap_, (std::size_t)size.width);
    top_line_ += scroll_offset_to_cursor_index(cursor_index_, line_lengths(spans_),
                                               top_line_, size.height);
}

void link(TextBox& tb, ScrollBar& sb)
{
    tb.on_scroll_update.connect(tracked(
        [](ScrollBar& sb, int pos, int len) {
            sb.position = pos;
            sb.scrollable_length = len;
        },
        sb));
    sb.on_scroll.connect(
        tracked([](TextBox& tb, int pos) { tb.set_top_line(pos); }, tb));
}

}  // namespace ox
