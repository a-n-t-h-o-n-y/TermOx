#ifndef DEMOS_DEMO_MENU_HPP
#define DEMOS_DEMO_MENU_HPP
#include <termox/painter/palette/apple_ii.hpp>
#include <termox/system/key.hpp>
#include <termox/system/shortcuts.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/menu_stack.hpp>
#include <termox/widget/widgets/titlebar.hpp>

#include "animation_demo.hpp"
#include "color_sliders.hpp"
#include "focus_demo.hpp"
#include "fractal/fractal_demo.hpp"
#include "game_of_life/gol_demo.hpp"
#include "glyph_paint/glyph_paint.hpp"
#include "graph/graph_demo.hpp"
#include "layout_demo.hpp"
#include "notepad/notepad.hpp"
#include "palette_creator.hpp"
#include "palette_view.hpp"
#include "snake/snake.hpp"
#include "two_lists.hpp"

namespace demo {

namespace Menu_palette = ox::apple_ii;

class Demo_menu : public ox::Menu_stack {
   public:
    Demo_menu()
    {
        using namespace ox;
        using namespace ox::pipe;

        ox::Terminal::set_palette(Menu_palette::palette);

        Shortcuts::add_shortcut(Key::Escape).connect([this] {
            ox::Terminal::set_palette(Menu_palette::palette);
            this->Menu_stack::goto_menu();
        });

        auto constexpr brush = ox::Brush{fg(ox::apple_ii::Aqua)};

        this->make_page<fractal::Fractal_demo>(U"Fractals" | brush);
        this->make_page<snake::Snake_game>(U"Snake Game" | brush);
        this->make_page<gol::GoL_demo>(U"Game of Life" | brush);
        // this->make_page<Notepad>(U"Notepad" | brush); // seg-faults on Load
        this->append_page(U"Notepad" | brush, notepad());
        this->make_page<paint::Glyph_paint>(U"Glyph Paint" | brush);
        this->append_page(U"Animated Widget" | brush, make_animation_demo());
        this->append_page(U"Focus" | brush, make_focus_demo());
        this->make_page<colors::Palette_view>(U"Color Palettes" | brush);
        this->append_page(U"Palette Creator" | brush, make_palette_creator());
        this->make_page<graph::Graph_demo>(U"Graph" | brush);
        this->make_page<Two_lists>(U"Check Lists" | brush);
        this->make_page<Color_sliders<50>>(
            U"Color Sliders" | brush,
            make_palette<50>({0, 100, 45}, {360, 100, 45}));

        // this->make_page<Layout_demo>(U"Layouts");
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
        using namespace ox::pipe;
        *this | direct_focus() | forward_focus(menu);

        back_bar.back_btn.pressed.connect([&] {
            ox::Terminal::set_palette(Menu_palette::palette);
            menu.goto_menu();
        });
    }
};

}  // namespace demo
#endif  // DEMOS_DEMO_MENU_HPP
