#include "main_menu.hpp"

#include <cppurses/cppurses.hpp>

using namespace cppurses;

namespace demo {

Main_menu::Main_menu() {
    this->initialize();
}

void Main_menu::initialize() {
    enable_border(main_menu);
    // main_menu.border.south_enabled = false;
    main_menu.border.north_enabled = false;
    // main_menu.border.south_west_enabled = false;
    // main_menu.border.south_east_enabled = false;
    main_menu.border.north_west_enabled = false;
    main_menu.border.north_east_enabled = false;
    hl.height_policy.type(Size_policy::Fixed);
    hl.height_policy.hint(1);

    set_background(titlebar, Color::Light_gray);
}

}  // namespace demo
