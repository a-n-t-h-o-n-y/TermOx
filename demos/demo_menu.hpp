#ifndef DEMOS_DEMO_MENU_HPP
#define DEMOS_DEMO_MENU_HPP
#include <termox/painter/detail/screen_descriptor.hpp>
#include <termox/painter/palette/dawn_bringer16.hpp>
#include <termox/system/key.hpp>
#include <termox/system/shortcuts.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/widgets/menu_stack.hpp>
#include <termox/widget/widgets/titlebar.hpp>

#include "animation/animation_demo.hpp"
#include "colors/palette_view.hpp"
#include "composites/composites.hpp"
#include "focus/focus_demo.hpp"
#include "game_of_life/gol_demo.hpp"
#include "glyph_paint/glyph_paint.hpp"
#include "graph/graph_demo.hpp"
#include "layout/layout_demo.hpp"
#include "notepad/notepad.hpp"
#include "palette/palette_demo.hpp"
#include "snake/snake.hpp"

namespace demos {

class Demo_menu : public ox::Menu_stack {
   public:
    Demo_menu() : Menu_stack{U"Demos"}
    {
        using namespace ox;
        using namespace ox::pipe;

        Shortcuts::add_shortcut(Key::Escape).connect([this] {
            System::terminal.set_palette(ox::dawn_bringer16::palette);
            this->Menu_stack::goto_menu();
        });

        // this->make_page<comp::Idea>(U"Idea");

        // clang-format off
        // this->append_page(U"Composites",
        //     layout::vertical
        //     (
        //         ox::make<Checkbox1>(),
        //         textbox(U"WOW!") | bg(Color::Green) | Trait::Bold,
        //         layout::horizontal
        //         (
        //             textbox(U"Left") | bg(Color::Light_blue),
        //             textbox(U"Right") | bg(Color::Violet)
        //         ),
        //         make<Checkbox2>(),
        //         hcheckbox_label({U"A Box"})
        //     )
        // );
        // clang-format on

        this->make_page<snake::Snake_game>(U"Snake Game");
        this->make_page<gol::GoL_demo>(U"Game of Life");
        this->make_page<Notepad>(U"Notepad");
        this->make_page<paint::Glyph_paint>(U"Glyph Paint");
        this->append_page(U"Animated Widget", animation::build_demo());
        this->append_page(U"Focus", focus::build_demo());
        this->make_page<colors::Palette_demo>(U"Color Palettes");
        this->make_page<graph::Graph_demo>(U"Graph");
        this->make_page<comp::Two_lists>(U"Check Lists");

        // this->make_page<palette::Palette_demo>(U"Color Palette");
        // this->make_page<layout_demo::Layout_demo>(U"Layouts");
    }
};

class Demos : public ox::layout::Vertical<> {
   public:
    // TODO add a back button to left side of titlebar that appears in demos
    using Titlebar = ox::Titlebar;

    Titlebar& title_ = this->make_child<Titlebar>("~ TermOx ~");
    Demo_menu& menu  = this->make_child<Demo_menu>();

   public:
    Demos() { this->focus_policy = ox::Focus_policy::Direct; }

   protected:
    auto focus_in_event() -> bool override
    {
        ox::System::set_focus(menu);
        return true;
    }
};

}  // namespace demos
#endif  // DEMOS_DEMO_MENU_HPP
