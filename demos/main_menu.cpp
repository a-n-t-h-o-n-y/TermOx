#include "main_menu.hpp"

#include <cppurses/system/events/focus_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/layouts/vertical.hpp>

using namespace cppurses;
namespace demos {

Main_menu::Main_menu() {
    this->set_name("Main_menu - head widget");
    this->focus_policy = Focus_policy::Strong;
    titlebar.set_name("Titlebar in Main_menu");
    main_menu.set_name("Menu_stack in Main_menu");
    main_menu.menu().set_name("Menu in Main Widget Stack Menu");
}

bool Main_menu::focus_in_event() {
    System::post_event<Focus_in_event>(main_menu);
    return layout::Vertical::focus_in_event();
}
}  // namespace demos
