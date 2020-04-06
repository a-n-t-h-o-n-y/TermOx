#ifndef DEMOS_DEMOS_HPP
#define DEMOS_DEMOS_HPP
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/shortcuts.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/menu_stack.hpp>
#include <cppurses/widget/widgets/titlebar.hpp>

#include "animation/animated_widget.hpp"
#include "chess/src/chess_ui.hpp"
#include "composites/composites.hpp"
#include "focus/focus_demo.hpp"
#include "game_of_life/gol_demo.hpp"
#include "glyph_paint/glyph_paint.hpp"
#include "graph/graph_demo.hpp"
#include "layout/layout_demo.hpp"
#include "notepad/notepad.hpp"
#include "palette/palette_demo.hpp"

namespace demos {

class Demo_menu : public cppurses::Menu_stack {
   public:
    Demo_menu() : Menu_stack{"D e m o s"}
    {
        using namespace cppurses;
        Shortcuts::add_shortcut(Key::Escape).connect([this] {
            this->Menu_stack::goto_menu();
        });

        this->make_page<comp::Composites>("Composites");
        this->make_page<graph::Graph_demo>("Graph");
        this->make_page<layout_demo::Layout_demo>("Layouts");
        this->make_page<Notepad>("Notepad");
        this->make_page<gol::GoL_demo>("Game of Life");
        this->make_page<palette::Palette_demo>("Color Palette");
        this->make_page<Chess_UI>("Chess");
        this->make_page<focus::Focus_demo>("Focus");
        this->make_page<glyph_paint::Glyph_paint>("Glyph Paint");
        this->make_page<animation::Animated_widget>("Animated Widget");
    }
};

class Demos : public cppurses::layout::Vertical<> {
   public:
    using Titlebar = cppurses::Titlebar;

    Titlebar& title_ = this->make_child<Titlebar>("  C  P  P  U  R  S  E  S");
    Demo_menu& menu  = this->make_child<Demo_menu>();

   public:
    Demos() { this->focus_policy = cppurses::Focus_policy::Direct; }

   protected:
    auto focus_in_event() -> bool override
    {
        cppurses::System::set_focus(menu);
        return cppurses::layout::Vertical<>::focus_in_event();
    }
};

}  // namespace demos
#endif  // DEMOS_DEMOS_HPP
