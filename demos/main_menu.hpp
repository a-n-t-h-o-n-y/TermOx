#ifndef DEMOS_MAIN_MENU_HPP
#define DEMOS_MAIN_MENU_HPP
#include "cppurses.hpp"
#include "notepad.hpp"

using namespace cppurses;
namespace demo {

class Main_menu : public Horizontal_layout {
   private:
    Widget_stack_menu& main_menu_{this->make_child<Widget_stack_menu>()};
    Notepad& notepad_{main_menu_.make_page<Notepad>("Notepad")};
};

}  // namespace demo

#endif  // DEMOS_MAIN_MENU_HPP
