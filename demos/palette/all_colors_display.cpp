#include "all_colors_display.hpp"

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/painter.hpp>

namespace palette {

bool Shade_display::paint_event()
{
    using namespace ox;
    auto light_shade   = L'░' | bg(base_);
    auto mid_shade     = L'▒' | bg(base_);
    auto const color_n = ox::System::terminal.get_palette_color_count();

    int const height = static_cast<int>(this->height());
    int const width  = static_cast<int>(this->width());

    int y_begin = inverted_ ? 0 : height - 1;
    auto y_end  = [this, height](int y) {
        return inverted_ ? y < height : y >= 0;
    };
    auto increment = [this](int& y) { inverted_ ? ++y : --y; };

    auto p      = Painter{*this};
    auto& shade = light_shade;
    for (auto y = y_begin, i = 0; y_end(y); increment(y)) {
        for (auto x = 0; x < width && i < (2 * color_n); ++x, ++i) {
            if (i == color_n)
                shade = mid_shade;
            auto const fg = Color{static_cast<Color::Value_t>(i % color_n)};
            if (fg == base_)
                --x;
            else {
                shade.brush.foreground = fg;
                p.put(shade, x, y);
            }
        }
    }
    return Widget::paint_event();
}

Bottom_shades::Bottom_shades()
{
    blue_shade.invert();
    orange_shade.invert();
    light_gray_shade.invert();
    light_green_shade.invert();
    violet_shade.invert();
    light_blue_shade.invert();
    yellow_shade.invert();
    white_shade.invert();
}

All_colors_display::All_colors_display()
{
    auto const width = 48;
    color_select.width_policy.maximum(width);
    color_select.height_policy.maximum(6);
    top_shades.width_policy.maximum(width);
    bottom_shades.width_policy.maximum(width);
}

}  // namespace palette
