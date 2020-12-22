#ifndef TERMOX_DEMOS_PALETTE_PALETTE_DEMOS_HPP
#define TERMOX_DEMOS_PALETTE_PALETTE_DEMOS_HPP
#include <termox/widget/layouts/horizontal.hpp>

#include "all_colors_display.hpp"
#include "color_definition_setter.hpp"

namespace palette {

/// TODO set aside black and white for utility colors that are not part of the
// palette, create a new palette, append new colors, define those colors with
// tools that let you set rgb or hsl or hex values, or a box for dynamic colors,
// rainbow, or fades with parameters editable. give names to each color,
// otherwise numbers are used, default the name to the number for the display.
// save palette by creating a text file .hpp for the palette(with palette name
// as file name) just don't save the black and white utility colors.

/// Provides a user interface to the color palette system.
class Palette_demo : public ox::layout::Horizontal<> {
   public:
    Palette_demo()
    {
        all_colors_display_.color_select.color_selected.connect(
            [this](ox::Color c) { color_setter_.change_current_color(c); });
    }

   private:
    Color_definition_setter& color_setter_ =
        this->make_child<Color_definition_setter>();
    All_colors_display& all_colors_display_ =
        this->make_child<All_colors_display>();
};

}  // namespace palette
#endif  // TERMOX_DEMOS_PALETTE_PALETTE_DEMOS_HPP
