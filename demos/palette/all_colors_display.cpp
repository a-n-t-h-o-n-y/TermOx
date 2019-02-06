#include "all_colors_display.hpp"

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/widget/border.hpp>

namespace palette {

Shade_display::Shade_display(cppurses::Color base) : base_{base} {}

void Shade_display::invert() {
    inverted_ = true;
}

bool Shade_display::paint_event() {
    using namespace cppurses;
    Glyph light_shade{L'░', background(base_)};
    Glyph mid_shade{L'▒', background(base_)};
    const auto color_n = 16;

    int y_begin = inverted_ ? 0 : this->height() - 1;
    auto y_end = [this](int y) {
        return inverted_ ? y < this->height() : y >= 0;
    };
    auto increment = [this](int& y) { inverted_ ? ++y : --y; };

    Painter p{*this};
    auto& shade = light_shade;
    for (auto y = y_begin, i = 0; y_end(y); increment(y)) {
        for (auto x = 0; x < this->width() && i < (2 * color_n); ++x, ++i) {
            if (i == color_n) {
                shade = mid_shade;
            }
            auto foreground =
                static_cast<Color>(detail::first_color_value + (i % 16));
            if (foreground == base_) {
                --x;
            } else {
                shade.brush.set_foreground(foreground);
                p.put(shade, x, y);
            }
        }
    }
    return Widget::paint_event();
}

Bottom_shades::Bottom_shades() {
    blue_shade.invert();
    orange_shade.invert();
    light_gray_shade.invert();
    light_green_shade.invert();
    violet_shade.invert();
    light_blue_shade.invert();
    yellow_shade.invert();
    white_shade.invert();
}

All_colors_display::All_colors_display() {
    const auto width = 48;

    color_select.width_policy.type(cppurses::Size_policy::Maximum);
    color_select.width_policy.hint(width);
    color_select.height_policy.type(cppurses::Size_policy::Maximum);
    color_select.height_policy.hint(6);

    top_shades.width_policy.type(cppurses::Size_policy::Maximum);
    top_shades.width_policy.hint(width);

    bottom_shades.width_policy.type(cppurses::Size_policy::Maximum);
    bottom_shades.width_policy.hint(width);
}

}  // namespace palette
