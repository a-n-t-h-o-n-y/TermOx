#include "main_menu.hpp"

#include <cppurses/system/system.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/layouts/vertical.hpp>

using namespace cppurses;
namespace demos {

Main_menu::Main_menu() {
    this->set_name("Main_menu - head widget");
    titlebar.set_name("Titlebar in Main_menu");
    main_menu.set_name("Menu_stack in Main_menu");
    main_menu.menu().set_name("Menu in Main Widget Stack Menu");
}
}  // namespace demos
