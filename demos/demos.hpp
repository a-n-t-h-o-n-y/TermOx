#ifndef DEMOS_DEMOS_HPP
#define DEMOS_DEMOS_HPP
#include <cppurses/painter/palette/dawn_bringer16.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/shortcuts.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/menu_stack.hpp>
#include <cppurses/widget/widgets/titlebar.hpp>

#include "animation/animated_widget.hpp"
#include "chess/src/chess_ui.hpp"
#include "colors/palette_view.hpp"
#include "composites/composites.hpp"
#include "focus/focus_demo.hpp"
#include "game_of_life/gol_demo.hpp"
#include "glyph_paint/glyph_paint.hpp"
#include "graph/graph_demo.hpp"
#include "layout/layout_demo.hpp"
#include "notepad/notepad.hpp"
#include "palette/palette_demo.hpp"

#include <signals/signal.hpp>

#include <cppurses/widget/layouts/vertical.hpp>

namespace demos {

class Demo_menu : public cppurses::Menu_stack {
   public:
    Demo_menu() : Menu_stack{"D e m o s"}
    {
        using namespace cppurses;
        Shortcuts::add_shortcut(Key::Escape).connect([this] {
            this->Menu_stack::goto_menu();
        });

        // TODO make overload that determines the type of Widget_t instead of
        // Args.. and moves the widget in insert() method that takes a
        // Widget_t&& instead of creating a new object. But move constructor
        // will invalidate any signals with references. Also once you have the
        // destroyed signal wired back up it would stop functionality but it
        // wouldn't crash.
        // this->make_page<layout::Vertical<>>("Composites",
        //     layout::Vertical<>
        //     {
        //         Checkbox{},
        //         Labeled_checkbox{"A Box"}
        //     }
        // );

        using namespace pipe;

        // clang-format off
        this->append_page("Composites",
            make<layout::Vertical<>>
            (
                make<Checkbox>(),
                make<Checkbox>(),
                make<Textbox>("WOW!") | bg(Color::Green),
                make<layout::Horizontal<>>
                (
                    make<Textbox>("Left") | bg(Color::Light_blue),
                    make<Textbox>("Right") | bg(Color::Violet)
                ),
                make<Checkbox>(),
                make<Labeled_checkbox>("A Box")
            ));
        // clang-format on

        // Can use find() methods and name() pipe to connect signals after
        // you build up the widget to quick prototype.

        this->make_page<colors::Palette_view>("Colors",
                                              dawn_bringer16::palette);
        this->make_page<comp::Two_lists>("Check Lists");
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
        return true;
    }
};

}  // namespace demos
#endif  // DEMOS_DEMOS_HPP
