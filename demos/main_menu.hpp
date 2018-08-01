#ifndef DEMOS_MAIN_MENU_HPP
#define DEMOS_MAIN_MENU_HPP
#include <cppurses/cppurses.hpp>

#include "animation/animated_widget.hpp"
#include "chess/src/chess_ui.hpp"
#include "glyph_paint/glyph_paint.hpp"
#include "notepad/notepad.hpp"

using namespace cppurses;
namespace demos {

class Main_menu : public Vertical_layout {
   public:
    Main_menu();

    Horizontal_layout& hl{this->make_child<Horizontal_layout>()};
    Titlebar& titlebar{hl.make_child<Titlebar>("  C  P  P  U  R  S  E  S")};
    Widget_stack_menu& main_menu{
        this->make_child<Widget_stack_menu>("D e m o s")};
    Notepad& notepad{main_menu.make_page<Notepad>("Notepad")};
    Chess_UI& chess{main_menu.make_page<Chess_UI>("Chess")};
    glyph_paint::Glyph_paint& glyph_paint{
        main_menu.make_page<glyph_paint::Glyph_paint>("Glyph Paint")};
    // animation::Animated_widget& animated_widget{
    //     main_menu.make_page<animation::Animated_widget>("Animated Widget")};

   private:
    void initialize();
};

}  // namespace demos

#endif  // DEMOS_MAIN_MENU_HPP
