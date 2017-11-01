#ifndef DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
#define DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
#include "attribute_box.hpp"
#include "options_box.hpp"
#include "populated_glyph_stack.hpp"

#include <cppurses/cppurses.hpp>

using namespace cppurses;

namespace demos {
namespace glyph_paint {

struct Side_pane : Vertical_layout {
    Side_pane();
    Populated_glyph_stack& glyph_select{
        this->make_child<Populated_glyph_stack>()};
    Blank_height& space1{this->make_child<Blank_height>(1)};

    Cycle_stack& color_select_stack{this->make_child<Cycle_stack>()};
    Color_select& color_select_foreground{
        color_select_stack.make_page<Color_select>(
            Glyph_string{"Foreground", Attribute::Bold})};
    Color_select& color_select_background{
        color_select_stack.make_page<Color_select>(
            Glyph_string{"Background", Attribute::Bold})};

    Attribute_box& attribute_box{this->make_child<Attribute_box>()};

    Status_bar& show_glyph{this->make_child<Status_bar>("x")};

    Blank_height& space2{this->make_child<Blank_height>(1)};
    Options_stack& options_box{this->make_child<Options_stack>()};
};

}  // namespace glyph_paint
}  // namespace demos
#endif  // DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
