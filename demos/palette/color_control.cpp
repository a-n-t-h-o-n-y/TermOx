#include "color_control.hpp"

#include <cppurses/widget/border.hpp>
#include <cppurses/widget/widget_free_functions.hpp>

#include "palette_helpers.hpp"

namespace {
using namespace cppurses;
/// Enable only the east wall of given a border, \p b. Also enables the border.
void enable_east_wall(Border& b) {
    b.enabled = true;
    disable_walls(b);
    disable_corners(b);
    b.east_enabled = true;
}
}  // namespace

namespace palette {

Color_control::Color_control() {
    enable_east_wall(slider_red_.border);
    slider_red_.width_policy.hint(6);

    enable_east_wall(slider_green_.border);
    slider_green_.width_policy.hint(6);
}

void Color_control::set_sliders(cppurses::Color color) {
    auto palette = cppurses::System::terminal.current_palette();
    const auto& definition = detail::extract_definition(palette, color);
    this->set_red_slider(definition.values.red);
    this->set_green_slider(definition.values.green);
    this->set_blue_slider(definition.values.blue);
}

}  // namespace palette
