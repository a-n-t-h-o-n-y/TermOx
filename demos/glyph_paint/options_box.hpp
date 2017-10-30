#ifndef DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
#define DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
#include <cppurses/cppurses.hpp>

using namespace cppurses;

namespace demos {
namespace glyph_paint {

struct Options_A : public Vertical_layout {
    Options_A();

    Push_button& clone_btn{this->make_child<Push_button>("Clone Tool")};
    Checkbox& eraser_box{this->make_child<Checkbox>("Eraser")};
    Checkbox& cursor_box{this->make_child<Checkbox>("Show Cursor")};
    Checkbox& grid_box{this->make_child<Checkbox>("Show Grid")};
    Confirm_button& clear_btn{
        this->make_child<Confirm_button>("Clear Painting")};
    Push_button& more_btn{this->make_child<Push_button>("More Options")};
};

struct Options_B : public Vertical_layout {
    Options_B();

    Label& palette_label{this->make_child<Label>("Color  Palette")};
    Cycle_box& palette_box{this->make_child<Cycle_box>()};

    Blank_height& space1{this->make_child<Blank_height>(1)};

    Save_file<char>& save_file{this->make_child<Save_file<char>>()};
    Open_file<char>& open_file{this->make_child<Open_file<char>>()};

    Push_button& back_btn{this->make_child<Push_button>("Back")};
};

struct Options_stack : public Widget_stack {
    Options_stack();

    Options_A& options_a{this->make_page<Options_A>()};
    Options_B& options_b{this->make_page<Options_B>()};
};

}  // namespace glyph_paint
}  // namespace demos

#endif  // DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
