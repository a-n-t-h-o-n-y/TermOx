#ifndef DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
#define DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
#include <cppurses/cppurses.hpp>

using namespace cppurses;

namespace demos {
namespace glyph_paint {

struct Options_box : public Vertical_layout {
    Options_box();
    Push_button& clone_btn{this->make_child<Push_button>("Clone Tool")};
    Checkbox& eraser_box{this->make_child<Checkbox>("Eraser")};
    Checkbox& cursor_box{this->make_child<Checkbox>("Show Cursor")};
    Confirm_button& clear_btn{
        this->make_child<Confirm_button>("Clear Painting")};

    Label& palette_label{this->make_child<Label>("Color Palette")};
    Cycle_box& palette_box{this->make_child<Cycle_box>()};

    Blank_height& space1{this->make_child<Blank_height>(1)};
    Horizontal_layout& save_layout{this->make_child<Horizontal_layout>()};
    Push_button& save_btn{save_layout.make_child<Push_button>("Save")};
    Blank_width& save_seperator{save_layout.make_child<Blank_width>(1)};
    Line_edit& save_filename_edit{
        save_layout.make_child<Line_edit>("Filename")};
    Horizontal_layout& open_layout{this->make_child<Horizontal_layout>()};
    Push_button& open_btn{open_layout.make_child<Push_button>("Open")};
    Blank_width& open_seperator{open_layout.make_child<Blank_width>(1)};
    Line_edit& open_filename_edit{
        open_layout.make_child<Line_edit>("Filename")};
};

}  // namespace glyph_paint
}  // namespace demos

#endif  // DEMOS_GLYPH_PAINT_OPTIONS_BOX_HPP
