#include <cppurses/system/focus.hpp>
#include <cppurses/system/system.hpp>

#include "main_menu.hpp"

using namespace cppurses;
int main() {
    System sys;
    demos::Main_menu demo_menu;
    sys.set_head(&demo_menu);
    Focus::set_focus_to(&demo_menu.main_menu.menu());
    return sys.run();
}
