#include "color_control.hpp"

#include <termox/system/system.hpp>
#include <termox/widget/border.hpp>

#include "palette_helpers.hpp"

namespace {
using namespace ox;

/// Enable only the east wall of given a border, \p b. Also enables the border.
void enable_east_wall(Border& b)
{
    b.enable();
    b.segments.disable_all();
    b.segments.east.enable();
}
}  // namespace

namespace palette {

Color_control::Color_control()
{
    enable_east_wall(slider_red_.border);
    slider_red_.width_policy.hint(6);

    enable_east_wall(slider_green_.border);
    slider_green_.width_policy.hint(6);
}

void Color_control::set_sliders(ox::Color /* color */)
{
    // auto palette           = ox::System::terminal.current_palette();
    // const auto& definition = detail::extract_definition(palette, color);
    // this->set_red_slider(definition.values.red);
    // this->set_green_slider(definition.values.green);
    // this->set_blue_slider(definition.values.blue);
}

}  // namespace palette
