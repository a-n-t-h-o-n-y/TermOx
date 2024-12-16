#include <ox/textbox.hpp>

#include <algorithm>
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
 * @details Each entry in the returned vector is a span of Glyphs that fit
 * within the given width. If the width is 0, then an empty vector is returned. This is
 * the const version.
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
    auto span = std::span<Glyph const>{glyphs};

    while (!span.empty()) {
        auto const max_count = std::min(std::size(span), width);

        auto const count_to_newline =
            (std::size_t)std::distance(std::cbegin(span),
                                       std::ranges::find(span, U'\n', &Glyph::symbol)) +
            1;

        auto split_count = std::min(count_to_newline, max_count);

        // If word-wrapping, search for space char nearest the end of the line and
        // re-set split_count to that index.
        if (wrap == TextBox::Wrap::Word && split_count == width) {
            auto const last_space_iter = [&] {
                auto const reverse_split_iter = std::next(
                    std::rbegin(span), (std::ptrdiff_t)(std::size(span) - width));
                return std::find_if(reverse_split_iter, std::rend(span), is_space);
            }();

            auto const distance_to_last_space =
                (std::size_t)std::distance(last_space_iter, std::rend(span));
            split_count =
                (distance_to_last_space > 0) ? distance_to_last_space : split_count;
        }

        spans.push_back(span.subspan(0, split_count));
        span = span.subspan(split_count);
    }

    // Add empty line if newline is last char
    if (!glyphs.empty() && glyphs.back().symbol == U'\n') {
        spans.push_back({});
    }

    return spans;
}

/**
 * Convert a text index to a screen position.
 *
 * @param index The index in the text to convert.
 * @param line_lengths The lengths of each line in the text.
 * @param top_line The index of the top line of the text box.
 * @return The screen position of the index, or std::nullopt if the index would be off
 * screen.
 */
template <std::ranges::range R>
[[nodiscard]] auto index_to_screen(int index,
                                   R&& line_lengths,
                                   int top_line,
                                   bool is_last) -> std::optional<Point>
{
    auto const line_count = std::size(line_lengths);
    auto point = Point{.x = (int)index, .y = -1 * top_line};
    for (auto const [count, length] : enumerate(line_lengths)) {
        if (point.x < length || length == 0) {
            if (point.y < 0) {
                return std::nullopt;
            }
            return point;
        }
        if (is_last && point.x == length && count + 1 == line_count) {
            return point;
        }
        point.x -= (int)length;
        point.y += 1;
    }
    return std::nullopt;
}

/**
 * Convert a screen position to a text index.
 */
template <std::ranges::range R>
[[nodiscard]] auto screen_to_index(Point p, R&& line_lengths, int top_line)
    -> std::size_t
{
    auto const y = top_line + std::min(p.y, (int)std::ssize(line_lengths) - 1);
    auto index = std::size_t{0};
    for (auto i = 0; i < y; ++i) {
        index += (std::size_t)line_lengths[i];
    }
    return index + (std::size_t)std::min(p.x, (int)line_lengths[y] - 1);
}

/**
 * Convert a text index to a (vertical) line index.
 *
 * @param index The index in the text to convert.
 * @param line_lengths The lengths of each line in the text.
 * @return The line index of the index, like top_line.
 */
template <std::ranges::range R>
[[nodiscard]] auto index_to_line(int index, R&& line_lengths) -> int
{
    auto y = 0;
    for (auto const length : line_lengths) {
        if (index <= length) {
            return y;
        }
        index -= length;
        ++y;
    }
    return y;
}

}  // namespace

namespace ox {

TextBox::Init const TextBox::init = {};

TextBox::TextBox(Init state)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      text{std::move(state.text)},
      wrap{state.wrap},
      align{state.align},
      background{state.background},
      insert_brush{state.insert_brush}
{}

void TextBox::paint(Canvas c)
{
    Painter{c}.fill(U' ' | bg(background));

    auto const spans =
        calculate_spans(this->text, this->wrap, (std::size_t)c.size.width);

    // Always emitted, but could be updated to be smarter about this.
    line_count_ = (int)std::size(spans);
    top_line = std::clamp(0, top_line, std::max(line_count_ - 1, 0));
    this->on_scroll_update(top_line, line_count_);

    for (auto i = top_line; i < std::ssize(spans) && (i - top_line) < c.size.height;
         ++i) {
        auto const span = spans[(std::size_t)i];

        auto const x = [&]() -> int {
            switch (align) {
                case Align::Left: return 0;
                case Align::Center: return (c.size.width - (int)span.size()) / 2;
                case Align::Right: return c.size.width - (int)span.size();
                default: throw std::logic_error{"Invalid Align"};
            }
        }();

        // Overwrite bg if Glyph's bg is XColor::Default.
        auto writer = Painter{c}[{x, i - top_line}];
        for (Glyph g : span) {
            if (g.symbol == U'\n') {
                continue;
            }
            if (g.brush.background == Color{XColor::Default}) {
                g.brush.background = background;
            }
            writer << g;
        }
    }

    auto cursor =
        index_to_screen((int)cursor_index_,
                        spans | std::ranges::views::take(top_line + c.size.height) |
                            std::ranges::views::transform(
                                [](auto const& span) { return std::ssize(span); }),
                        top_line, cursor_index_ == std::size(this->text));

    // Adjust x for Alignment
    if (cursor.has_value()) {
        auto const span = spans[(std::size_t)(top_line + cursor->y)];
        auto const x = [&]() -> int {
            switch (align) {
                case Align::Left: return 0;
                case Align::Center: return (c.size.width - (int)span.size()) / 2;
                case Align::Right: return c.size.width - (int)span.size();
                default: throw std::logic_error{"Invalid Align"};
            }
        }() + cursor->x;
        cursor = Point{x, cursor->y};
    }
    Widget::cursor = std::move(cursor);
}

void TextBox::key_press(Key k)
{
    // In case user modified text or cursor_index.
    cursor_index_ = std::min(cursor_index_, text.size());

    switch (k) {
        case Key::Backspace:
            if (!text.empty() && cursor_index_ > 0) {
                --cursor_index_;
                text.erase(std::next(std::cbegin(text), (std::ptrdiff_t)cursor_index_));
            }
            break;
        case Key::Enter:
            text.insert(std::next(std::cbegin(text), (std::ptrdiff_t)cursor_index_++),
                        {U'\n'});
            break;
        case Key::Delete:
            if (cursor_index_ < text.size()) {
                text.erase(std::next(std::cbegin(text), (std::ptrdiff_t)cursor_index_));
            }
            break;
        case Key::ArrowLeft:
            if (cursor_index_ > 0) {
                --cursor_index_;
            }
            break;
        case Key::ArrowRight:
            if (cursor_index_ < text.size()) {
                ++cursor_index_;
            }
            break;
        case Key::ArrowDown:
            top_line = std::min(std::max(line_count_ - 1, 0), top_line + 1);
            break;
        case Key::ArrowUp: top_line = std::max(top_line - 1, 0); break;
        case Key::Home: cursor_index_ = 0; break;
        case Key::End: cursor_index_ = text.size(); break;
        default:
            if (auto const c = esc::key_to_char(k); c != '\0') {
                text.insert(std::next(std::begin(text), (std::ptrdiff_t)cursor_index_),
                            {
                                .symbol = (char32_t)c,
                                .brush = insert_brush,
                            });
                ++cursor_index_;
            }
            break;
    }
}

void TextBox::mouse_press(Mouse m)
{
    auto const spans =
        calculate_spans(this->text, this->wrap, (std::size_t)this->size.width);

    auto const line_lengths = spans |
                              std::ranges::views::take(top_line + this->size.height) |
                              std::ranges::views::transform(
                                  [](auto const& span) { return std::ssize(span); });

    auto const span = spans[(std::size_t)(top_line + m.at.y)];
    m.at.x = m.at.x - [&]() -> int {
        switch (align) {
            case Align::Left: return 0;
            case Align::Center: return (this->size.width - (int)span.size()) / 2;
            case Align::Right: return this->size.width - (int)span.size();
            default: throw std::logic_error{"Invalid Align"};
        }
    }();

    cursor_index_ = screen_to_index(m.at, line_lengths, top_line);
}

void TextBox::mouse_wheel(Mouse m)
{
    if (m.button == Mouse::Button::ScrollUp) {
        if (top_line > 0) {
            --top_line;
        }
    }
    else if (m.button == Mouse::Button::ScrollDown) {
        top_line = std::min(std::max(line_count_ - 1, 0), top_line + 1);
    }
}

void link(TextBox& tb, ScrollBar& sb)
{
    tb.on_scroll_update.connect(tracked(
        [](ScrollBar& sb, int pos, int len) {
            sb.position = pos;
            sb.scrollable_length = len;
        },
        sb));
    sb.on_scroll.connect(tracked([](TextBox& tb, int pos) { tb.top_line = pos; }, tb));
}

}  // namespace ox
