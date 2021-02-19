#ifndef DEMOS_DEMO_MENU_HPP
#define DEMOS_DEMO_MENU_HPP
#include <termox/painter/detail/screen_descriptor.hpp>
#include <termox/painter/palette/apple_ii.hpp>
#include <termox/system/key.hpp>
#include <termox/system/shortcuts.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/widgets/button.hpp>
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

namespace Menu_palette = ox::apple_ii;

class Demo_menu : public ox::Menu_stack {
   public:
    Demo_menu()
    {
        using namespace ox;
        using namespace ox::pipe;

        ox::System::terminal.set_palette(Menu_palette::palette);

        Shortcuts::add_shortcut(Key::Escape).connect([this] {
            ox::System::terminal.set_palette(Menu_palette::palette);
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
        auto constexpr brush = ox::Brush{fg(ox::apple_ii::Aqua)};

        this->make_page<snake::Snake_game>(U"Snake Game" | brush);
        this->make_page<gol::GoL_demo>(U"Game of Life" | brush);
        this->make_page<Notepad>(U"Notepad" | brush);
        this->make_page<paint::Glyph_paint>(U"Glyph Paint" | brush);
        this->append_page(U"Animated Widget" | brush, animation::build_demo());
        this->append_page(U"Focus" | brush, focus::build_demo());
        this->make_page<colors::Palette_demo>(U"Color Palettes" | brush);
        this->make_page<graph::Graph_demo>(U"Graph" | brush);
        this->make_page<comp::Two_lists>(U"Check Lists" | brush);

        // this->make_page<palette::Palette_demo>(U"Color Palette");
        // this->make_page<layout_demo::Layout_demo>(U"Layouts");
    }
};

class Back_bar : public ox::layout::Horizontal<> {
   public:
    ox::Button& back_btn =
        this->make_child<ox::Button>(U"< Back" | ox::Trait::Bold);
    ox::Widget& buffer = this->make_child() | ox::pipe::wallpaper(U'🮘');

   public:
    Back_bar()
    {
        *this | ox::pipe::fixed_height(1);
        back_btn | ox::pipe::fixed_width(7);
    }
};

class Demos : public ox::layout::Vertical<> {
   public:
    ox::Titlebar& title_ = this->make_child<ox::Titlebar>("~ TermOx Demos ~");
    Back_bar& back_bar   = this->make_child<Back_bar>();
    Demo_menu& menu      = this->make_child<Demo_menu>();

   public:
    Demos()
    {
        this->focus_policy = ox::Focus_policy::Direct;
        back_bar.back_btn.pressed.connect([&] {
            ox::System::terminal.set_palette(Menu_palette::palette);
            menu.goto_menu();
        });
    }

   protected:
    auto focus_in_event() -> bool override
    {
        ox::System::set_focus(menu);
        return true;
    }
};

}  // namespace demos
#endif  // DEMOS_DEMO_MENU_HPP
