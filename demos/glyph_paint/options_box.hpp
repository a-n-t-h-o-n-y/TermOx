#ifndef DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
#define DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widgets/blank_height.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>
#include <cppurses/widget/widgets/confirm_button.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/open_file.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/save_file.hpp>
#include <cppurses/widget/widgets/widget_stack.hpp>

namespace demos {
namespace glyph_paint {

struct Options_A : public cppurses::Vertical_layout {
    Options_A();

    cppurses::Push_button& clone_btn{
        this->make_child<cppurses::Push_button>("Clone Tool")};
    cppurses::Checkbox& eraser_box{
        this->make_child<cppurses::Checkbox>("Eraser")};
    cppurses::Checkbox& cursor_box{
        this->make_child<cppurses::Checkbox>("Show Cursor")};
    cppurses::Checkbox& grid_box{
        this->make_child<cppurses::Checkbox>("Show Grid")};
    cppurses::Confirm_button& clear_btn{
        this->make_child<cppurses::Confirm_button>("Clear Painting")};
    cppurses::Push_button& more_btn{
        this->make_child<cppurses::Push_button>("More Options")};
};

struct Options_B : public cppurses::Vertical_layout {
    Options_B();

    cppurses::Label& palette_label{
        this->make_child<cppurses::Label>("Color  Palette")};
    cppurses::Cycle_box& palette_box{this->make_child<cppurses::Cycle_box>()};

    cppurses::Blank_height& space1{this->make_child<cppurses::Blank_height>(1)};

    cppurses::Save_file<>& save_file{this->make_child<cppurses::Save_file<>>()};
    cppurses::Open_file<>& open_file{this->make_child<cppurses::Open_file<>>()};

    cppurses::Push_button& back_btn{
        this->make_child<cppurses::Push_button>("Back")};
};

struct Options_stack : public cppurses::Widget_stack {
    Options_stack();

    Options_A& options_a{this->make_child<Options_A>()};
    Options_B& options_b{this->make_child<Options_B>()};
};

}  // namespace glyph_paint
}  // namespace demos
#endif  // DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
