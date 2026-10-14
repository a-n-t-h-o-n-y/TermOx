#include <ox/label.hpp>

#include <cstddef>
#include <cstdint>
#include <locale>
#include <stdexcept>
#include <string>
#include <string_view>

#include <esc/text.hpp>

#include <ox/align.hpp>
#include <ox/core/core.hpp>
#include <ox/put.hpp>

namespace ox {

void put_label(Canvas canvas,
               std::string_view text,
               Align align,
               Style style,
               bool vertical)
{
    auto const unicode = esc::utf8_to_utf32(text);
    auto end = std::size_t{0};
    auto used = 0;
    auto fits = true;
    auto const available = vertical ? canvas.size().height : canvas.size().width;
    for (auto cluster : esc::graphemes(unicode)) {
        auto const cells = esc::grapheme_width(cluster, canvas.width_policy());
        if (!cells || *cells == 0) {
            throw std::invalid_argument{
                "Label requires printable text with known widths"};
        }
        auto const length = vertical ? std::size_t{1} : *cells;
        if (!fits || length > (std::size_t)(available - used) ||
            (vertical && *cells > (std::size_t)canvas.size().width)) {
            fits = false;
            continue;
        }
        used += (int)length;
        end += cluster.size();
    }
    auto const offset = find_align_offset(align, available, used);
    auto const prefix = std::u32string_view{unicode}.substr(0, end);
    if (!vertical) { put(canvas, {offset, canvas.size().height / 2}, prefix, style); }
    else {
        auto y = offset;
        for (auto cluster : esc::graphemes(prefix)) {
            auto const cells = *esc::grapheme_width(cluster, canvas.width_policy());
            canvas.write({(canvas.size().width - (int)cells) / 2, y++}, cluster, style);
        }
    }
}
void paint_label(Canvas canvas, std::string_view text, Align align, Style style)
{
    fill(canvas, U' ' | style);
    put_label(canvas, text, align, style);
}

Label::Options const Label::init = {};

Label::Label(Options x)
    : Widget{FocusPolicy::None, x.size_policy},
      text{std::move(x.text)},
      align{std::move(x.align)},
      style{std::move(x.style)}
{}

Label::Label(std::string text_)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      text{std::move(text_)},
      align{Align::Center},
      style{}
{}

Label::Label(char const* text_)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      text{text_},
      align{Align::Center},
      style{}
{}

auto Label::paint(Canvas c) -> Cursor
{
    paint_label(c, text, align, style);
    return {};
}

// -------------------------------------------------------------------------------------

auto digit_separator_locale(std::int8_t spacing, std::locale init_locale) -> std::locale
{
    struct facet : std::numpunct<char> {
        int digit_spacing;

        facet(int spacing) : digit_spacing{spacing} {}

        auto do_grouping() const -> std::string override
        { return std::string(1, static_cast<char>(digit_spacing)); }
    };
    if (spacing < 0) {
        throw std::invalid_argument{
            "digit_separator_locale: spacing must be non-negative"};
    }
    return std::locale{init_locale, new facet{spacing}};
}

}  // namespace ox