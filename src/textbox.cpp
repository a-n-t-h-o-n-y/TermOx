#include <ox/textbox.hpp>

#include <algorithm>
#include <optional>
#include <ranges>
#include <span>
#include <string_view>
#include <vector>

#include <ox/core/core.hpp>
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

    if (width == 0) { return {}; }

    auto spans = std::vector<std::span<Glyph const>>{};
    auto span = std::span<Glyph const>{glyphs};

    while (!span.empty()) {
        auto const max_count = std::min(std::size(span), width);

        auto const newline_count =
            (std::size_t)std::distance(std::cbegin(span),
                                       std::ranges::find(span, U'\n', &Glyph::symbol)) +
            1;

        auto split_count = std::min(newline_count, max_count);

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

    return spans;
}

/**
 * Convert a text index to a screen position.
 *
 * @param index The index in the text to convert.
 * @return The screen position of the index, or std::nullopt if the index would be off
 * screen.
 */
template <std::ranges::range R>
[[nodiscard]] auto index_to_screen(int index,
                                   R&& line_lengths,
                                   int top_line) -> std::optional<Point>
{
    auto y = -1 * top_line;
    for (auto const length : line_lengths) {
        if (index < length) {
            if (y < 0) { return std::nullopt; }
            return Point{(int)(index), y};
        }
        index -= length;
        ++y;
    }
    return std::nullopt;
}

}  // namespace

namespace ox {

void TextBox::paint(Canvas c)
{
    Painter{c}.fill(U' ' | bg(background));

    auto const spans =
        calculate_spans(this->text, this->wrap, (std::size_t)c.size.width);

    // TODO clamp top_line_ here and you don't have to worry about it anywhere else?
    // Because you need the spans to know where to clamp. (0, spans.size())
    // But do you also scroll if it isn't on screen? How does arrow down scroll if it
    // doesn't know where top_line_ is, if it if calculates scroll based on the invalid
    // top_line_ it has set?

    for (auto i = top_line_; i < std::ssize(spans) && (i - top_line_) < c.size.height;
         ++i) {
        auto const span = spans[(std::size_t)i];

        auto const x = [&]() -> int {
            switch (align) {
                case Align::Left: return 0;
                case Align::Center: return (c.size.width - (int)span.size()) / 2;
                case Align::Right: return c.size.width - (int)span.size();
            }
        }();

        // Overwrite bg if Glyph's bg is XColor::Default.
        auto writer = Painter{c}[{x, i - top_line_}];
        for (Glyph g : span) {
            if (g.symbol == U'\n') { continue; }
            if (g.brush.background == Color{XColor::Default}) {
                g.brush.background = background;
            }
            writer << g;
        }
    }

    auto cursor = index_to_screen(
        (int)cursor_index_, spans | std::ranges::views::transform([](auto const& span) {
                                return std::ssize(span);
                            }),
        top_line_);

    // Adjust x for Alignment
    if (cursor.has_value()) {
        auto const span = spans[(std::size_t)(top_line_ + cursor->y)];
        auto const x = [&]() -> int {
            switch (align) {
                case Align::Left: return 0;
                case Align::Center: return (c.size.width - (int)span.size()) / 2;
                case Align::Right: return c.size.width - (int)span.size();
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

    // TODO scroll if arrow key moves cursor out of view.

    // TODO
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
            // TODO scroll if needed
        case Key::ArrowLeft:
            if (cursor_index_ > 0) { --cursor_index_; }
            break;
        case Key::ArrowRight:
            if (cursor_index_ < text.size()) { ++cursor_index_; }
            break;
            // TODO these should scroll if needed
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

}  // namespace ox

// namespace ox {

// auto resize(TextBox& tb, Area new_size) -> void
// {
//     // In case user modified text or cursor_index.
//     tb.cursor_index = std::min(tb.cursor_index, tb.text.size());

//     tb.width = (std::size_t)new_size.width;
// }

// auto key_press(TextBox& tb, Key k) -> void
// {
//     // In case user modified text or cursor_index.
//     tb.cursor_index = std::min(tb.cursor_index, tb.text.size());

//     switch (k) {
//         case Key::Backspace:
//             if (!tb.text.empty() && tb.cursor_index <= tb.text.size()) {
//                 --tb.cursor_index;
//                 tb.text.erase(tb.cursor_index, 1);
//             }
//             break;
//         case Key::Enter: tb.text.insert(tb.cursor_index++, 1, '\n'); break;
//         case Key::Delete:
//             if (tb.cursor_index < tb.text.size()) {
//                 tb.text.erase(tb.cursor_index, 1);
//             }
//             break;
//         case Key::ArrowLeft:
//             if (tb.cursor_index > 0) {
//                 --tb.cursor_index;
//             }
//             break;
//         case Key::ArrowRight:
//             if (tb.cursor_index < tb.text.size()) {
//                 ++tb.cursor_index;
//             }
//             break;
//         case Key::Home: tb.cursor_index = 0; break;
//         case Key::End: tb.cursor_index = tb.text.size(); break;
//         default:
//             if (auto const c = key_to_char(k); c != '\0') {
//                 tb.text.insert(tb.cursor_index, 1, c);
//                 ++tb.cursor_index;
//             }
//             break;
//     }
// }

// }  // namespace ox