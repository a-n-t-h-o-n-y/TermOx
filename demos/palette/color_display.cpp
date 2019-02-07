#include "color_display.hpp"

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/size_policy.hpp>

using namespace cppurses;

namespace palette {

Color_patch::Color_patch(Color value) {
    this->set_color(value);
    this->width_policy.type(Size_policy::Maximum);
    this->width_policy.hint(5);
    this->height_policy.type(Size_policy::Maximum);
    this->height_policy.hint(2);
}

void Color_patch::set_color(Color value) {
    this->brush.set_background(value);
    this->update();
}

Color_display::Color_display(Color color) {
    this->height_policy.type(Size_policy::Maximum);
    this->height_policy.hint(3);

    this->border.enabled = true;
    disable_walls(this->border);
    disable_corners(this->border);
    this->border.south_enabled = true;

    this->set_color(color);
}

void Color_display::set_color(Color color) {
    text.name.set_contents(color_to_string(color));
    square.set_color(color);
}
}  // namespace palette
