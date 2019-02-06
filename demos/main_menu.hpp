#ifndef DEMOS_MAIN_MENU_HPP
#define DEMOS_MAIN_MENU_HPP
#include <cppurses/cppurses.hpp>

// #include "animation/animated_widget.hpp"
#include "chess/src/chess_ui.hpp"
#include "focus/focus_demo.hpp"
#include "game_of_life/gol_demo.hpp"
#include "glyph_paint/glyph_paint.hpp"
#include "notepad/notepad.hpp"
#include "palette/palette_demo.hpp"

using namespace cppurses;
namespace demos {

class Main_menu : public Vertical_layout {
   public:
    Main_menu();

    Titlebar& titlebar{this->make_child<Titlebar>("  C  P  P  U  R  S  E  S")};

    Widget_stack_menu& main_menu{
        this->make_child<Widget_stack_menu>("D e m o s")};

    Notepad& notepad{main_menu.make_page<Notepad>("Notepad")};

    gol::GoL_demo& game_of_life_demo{
        main_menu.make_page<gol::GoL_demo>("Game of Life")};

    palette::Palette_demo& palette_demo{
        main_menu.make_page<palette::Palette_demo>("Color Palette")};

    Chess_UI& chess{main_menu.make_page<Chess_UI>("Chess")};

    focus::Focus_demo& focus_demo{
        main_menu.make_page<focus::Focus_demo>("Focus")};

    glyph_paint::Glyph_paint& glyph_paint{
        main_menu.make_page<glyph_paint::Glyph_paint>("Glyph Paint")};

    // animation::Animated_widget& animated_widget{
    //     main_menu.make_page<animation::Animated_widget>(
    //         "Animated Widget(Experimental)")};
};

}  // namespace demos

#endif  // DEMOS_MAIN_MENU_HPP
