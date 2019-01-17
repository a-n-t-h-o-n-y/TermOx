#include "color_definition_setter.hpp"

#include <cppurses/painter/color.hpp>

#include "palette_helpers.hpp"

namespace palette {

// Initializes color control_object to set color definitions.
Color_definition_setter::Color_definition_setter() {
    using namespace cppurses;

    this->change_current_color(Color::White);
    this->width_policy.type(Size_policy::Maximum);
    this->width_policy.hint(18);

    this->border.enabled = true;
    disable_walls(this->border);
    disable_corners(this->border);
    this->border.east_enabled = true;

    using namespace palette::detail;
    color_control_.red_changed.connect(
        [this](int red_value) { set_red(current_color_, red_value); });
    color_control_.green_changed.connect(
        [this](int green_value) { set_green(current_color_, green_value); });
    color_control_.blue_changed.connect(
        [this](int blue_value) { set_blue(current_color_, blue_value); });
}

void Color_definition_setter::change_current_color(cppurses::Color color) {
    current_color_ = color;
    color_display_.set_color(color);
    color_control_.set_sliders(color);
}
}  // namespace palette
