#include "color_definition_setter.hpp"

#include <termox/painter/color.hpp>

#include "palette_helpers.hpp"

using namespace ox;

namespace palette {

// Initializes color control_object to set color definitions.
Color_definition_setter::Color_definition_setter()
{
    this->width_policy.maximum(18);
    this->border.enable();
    this->border.segments.disable_all();
    this->border.segments.east.enable();

    this->change_current_color(Color::White);

    using namespace palette::detail;
    // color_control_.red_changed.connect(
    //     [this](int red_value) { set_red(current_color_, red_value); });
    // color_control_.green_changed.connect(
    //     [this](int green_value) { set_green(current_color_, green_value); });
    // color_control_.blue_changed.connect(
    //     [this](int blue_value) { set_blue(current_color_, blue_value); });
}

void Color_definition_setter::change_current_color(Color color)
{
    current_color_ = color;
    color_display_.set_color(color);
    color_control_.set_sliders(color);
}

}  // namespace palette
