#include <ox/label.hpp>

#include <cstddef>
#include <cstdint>
#include <locale>
#include <stdexcept>
#include <string>
#include <string_view>

#include <ox/align.hpp>
#include <ox/core/core.hpp>

namespace ox::detail {
using namespace ox;

void paint_label(Canvas c, std::string_view text, Align align, Brush brush)
{
    auto painter = Painter{c};
    painter.fill(U' ' | bg(brush.background));

    auto glyphs = text | brush;  // Count visible space with Glyphs, not bytes.
    if (std::ssize(glyphs) > c.size.width) {
        glyphs.resize((std::size_t)c.size.width);
    }

    painter[{
        .x = find_align_offset(align, c.size.width, (int)glyphs.size()),
        .y = c.size.height / 2,
    }] << glyphs;
}

}  // namespace ox::detail

namespace ox {

Label::Options const Label::init = {};

Label::Label(Options x)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      text{std::move(x.text)},
      align{std::move(x.align)},
      brush{std::move(x.brush)}
{}

void Label::paint(Canvas c) { detail::paint_label(c, text, align, brush); }

// -------------------------------------------------------------------------------------

auto digit_separator_locale(std::int8_t spacing, std::locale init_locale) -> std::locale
{
    struct facet : std::numpunct<char> {
        int digit_spacing;

        facet(int spacing) : digit_spacing{spacing} {}

        auto do_grouping() const -> std::string override
        {
            return std::string(1, static_cast<char>(digit_spacing));
        }
    };
    if (spacing < 0) {
        throw std::invalid_argument{
            "digit_separator_locale: spacing must be non-negative"};
    }
    return std::locale{init_locale, new facet{spacing}};
}

}  // namespace ox