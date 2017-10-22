#ifndef DEMOS_MAIN_MENU_HPP
#define DEMOS_MAIN_MENU_HPP
#include <cppurses/cppurses.hpp>
#include "notepad/notepad.hpp"
#include "chess/src/chess_ui.hpp"

using namespace cppurses;
namespace demo {

class Main_menu : public Vertical_layout {
   public:
    Main_menu();

    Horizontal_layout& hl{this->make_child<Horizontal_layout>()};
    Titlebar& titlebar{hl.make_child<Titlebar>(" C (P P) U R S E S")};
    Widget_stack_menu& main_menu{this->make_child<Widget_stack_menu>()};
    Notepad& notepad{main_menu.make_page<Notepad>("Notepad")};
    Chess_UI& chess{main_menu.make_page<Chess_UI>("Chess")};

   private:
    void initialize();
};

}  // namespace demo

#endif  // DEMOS_MAIN_MENU_HPP
