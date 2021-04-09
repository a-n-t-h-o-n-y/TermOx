#ifndef DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
#define DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
#include <termox/painter/color.hpp>
#include <termox/widget/layouts/stack.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/checkbox.hpp>
#include <termox/widget/widgets/confirm_button.hpp>
#include <termox/widget/widgets/cycle_box.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/read_file.hpp>
#include <termox/widget/widgets/write_file.hpp>

namespace paint {

class Options_A : public ox::layout::Vertical<> {
   private:
    using Checkbox = ox::HCheckbox_label;

   public:
    ox::Button& clone_btn = make_child<ox::Button>(U"Clone Tool");
    Checkbox& eraser_box  = make_child<Checkbox>({U"Eraser"});
    Checkbox& cursor_box  = make_child<Checkbox>({U"Show Cursor"});
    Checkbox& grid_box    = make_child<Checkbox>({U"Show Grid"});
    ox::Confirm_button& clear_btn =
        make_child<ox::Confirm_button>(U"Clear Painting");
    ox::Button& more_btn = make_child<ox::Button>(U"More Options");

   public:
    Options_A()
    {
        using namespace ox::pipe;
        using ox::Color;
        clone_btn | bg(Color::White) | fg(Color::Black);
        more_btn | bg(Color::Light_gray) | fg(Color::Black);
        clear_btn.main_btn | bg(Color::White) | fg(Color::Black);
    }
};

class Options_B : public ox::layout::Vertical<> {
   public:
    ox::HLabel& palette_label    = make_child<ox::HLabel>({U"Color  Palette"});
    ox::Cycle_box& palette_box   = make_child<ox::Cycle_box>();
    Widget& space1               = make_child() | ox::pipe::fixed_height(1);
    ox::Write_file<>& write_file = make_child<ox::Write_file<>>();
    ox::Read_file<>& read_file   = make_child<ox::Read_file<>>();
    ox::Button& back_btn         = make_child<ox::Button>("Back");

   public:
    Options_B();
};

class Options_stack : public ox::layout::Stack<> {
   public:
    Options_A& options_a = make_child<Options_A>();
    Options_B& options_b = make_child<Options_B>();

   public:
    Options_stack()
    {
        using namespace ox::pipe;

        *this | active_page(0) | fixed_height(6);
        options_a.more_btn |
            on_press(ox::slot::set_active_page(*this, b_index_));
        options_b.back_btn |
            on_press(ox::slot::set_active_page(*this, a_index_));
    }

   private:
    static auto constexpr a_index_ = 0;
    static auto constexpr b_index_ = 1;
};

}  // namespace paint
#endif  // DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
