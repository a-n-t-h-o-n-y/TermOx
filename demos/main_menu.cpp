#include "main_menu.hpp"

#include <cppurses/cppurses.hpp>

using namespace cppurses;

namespace demos {

Main_menu::Main_menu() {
    this->initialize();
    this->set_name("Main_menu - head widget");
    titlebar.set_name("Titlebar in Main_menu");
    main_menu.set_name("Widget_stack_menu in Main_menu");

    // temp
    main_menu.menu().set_name("Menu in Main Widget Stack Menu");
}

void Main_menu::initialize() {
    // hl.height_policy.type(Size_policy::Fixed);
    // hl.height_policy.hint(1);
    titlebar.height_policy.type(Size_policy::Fixed);
    titlebar.height_policy.hint(1);
}

}  // namespace demos
