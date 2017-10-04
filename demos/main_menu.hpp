#ifndef DEMOS_MAIN_MENU_HPP
#define DEMOS_MAIN_MENU_HPP
#include "cppurses.hpp"
#include "notepad.hpp"

using namespace cppurses;
namespace demo {

class Main_menu : public Vertical_layout {
   public:
    Main_menu();

    Titlebar& titlebar{this->make_child<Titlebar>(" C (P P) U R S E S")};
    Widget_stack_menu& main_menu{this->make_child<Widget_stack_menu>()};
    Notepad& notepad{main_menu.make_page<Notepad>("Notepad")};

   private:
    void initialize();
};

}  // namespace demo

#endif  // DEMOS_MAIN_MENU_HPP
