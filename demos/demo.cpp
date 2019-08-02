#include <cppurses/system/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/widgets/menu.hpp>

#include "main_menu.hpp"

using namespace cppurses;

int main()
{
    System sys;
    demos::Main_menu demo_menu;
    System::set_initial_focus(&demo_menu.main_menu);
    return sys.run(demo_menu);
}
