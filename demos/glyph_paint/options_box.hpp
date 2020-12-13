#ifndef DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
#define DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>
#include <cppurses/widget/widgets/confirm_button.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/open_file.hpp>
#include <cppurses/widget/widgets/save_file.hpp>

namespace paint {

class Options_A : public cppurses::layout::Vertical<> {
   private:
    using Checkbox = cppurses::Labeled_checkbox;

   public:
    cppurses::Button& clone_btn = make_child<cppurses::Button>(L"Clone Tool");
    Checkbox& eraser_box        = make_child<Checkbox>(L"Eraser");
    Checkbox& cursor_box        = make_child<Checkbox>(L"Show Cursor");
    Checkbox& grid_box          = make_child<Checkbox>(L"Show Grid");
    cppurses::Confirm_button& clear_btn =
        make_child<cppurses::Confirm_button>(L"Clear Painting");
    cppurses::Button& more_btn = make_child<cppurses::Button>(L"More Options");

   public:
    Options_A()
    {
        using namespace cppurses::pipe;
        using cppurses::Color;
        clone_btn | bg(Color::White) | fg(Color::Black);
        more_btn | bg(Color::Light_gray) | fg(Color::Black);
        clear_btn.main_btn | bg(Color::White) | fg(Color::Black);
    }
};

class Options_B : public cppurses::layout::Vertical<> {
   public:
    cppurses::HLabel& palette_label =
        make_child<cppurses::HLabel>({L"Color  Palette"});
    cppurses::Cycle_box& palette_box = make_child<cppurses::Cycle_box>();
    Widget& space1 = make_child() | cppurses::pipe::fixed_height(1);
    cppurses::Save_file<>& save_file = make_child<cppurses::Save_file<>>();
    cppurses::Open_file<>& open_file = make_child<cppurses::Open_file<>>();
    cppurses::Button& back_btn       = make_child<cppurses::Button>("Back");

   public:
    Options_B();
};

class Options_stack : public cppurses::layout::Stack<> {
   public:
    Options_A& options_a = make_child<Options_A>();
    Options_B& options_b = make_child<Options_B>();

   public:
    Options_stack()
    {
        using namespace cppurses::pipe;

        *this | active_page(0) | fixed_height(6);
        options_a.more_btn |
            on_press(cppurses::slot::set_active_page(*this, b_index_));
        options_b.back_btn |
            on_press(cppurses::slot::set_active_page(*this, a_index_));
    }

   private:
    static auto constexpr a_index_ = 0;
    static auto constexpr b_index_ = 1;
};

}  // namespace paint
#endif  // DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
