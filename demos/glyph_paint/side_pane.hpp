#ifndef DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
#define DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
#include "attribute_box.hpp"
#include "options_box.hpp"
#include "populated_glyph_stack.hpp"

#include <cppurses/painter/attribute.hpp>
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widgets/blank_height.hpp>
#include <cppurses/widget/widgets/color_select.hpp>
#include <cppurses/widget/widgets/cycle_stack.hpp>
#include <cppurses/widget/widgets/status_bar.hpp>

namespace demos {
namespace glyph_paint {

struct Side_pane : cppurses::Vertical_layout {
    Side_pane();

    Populated_glyph_stack& glyph_select{
        this->make_child<Populated_glyph_stack>()};
    cppurses::Blank_height& space1{this->make_child<cppurses::Blank_height>(1)};

    cppurses::Cycle_stack& color_select_stack{
        this->make_child<cppurses::Cycle_stack>()};
    cppurses::Color_select& color_select_foreground{
        color_select_stack.make_page<cppurses::Color_select>(
            cppurses::Glyph_string{"Foreground", cppurses::Attribute::Bold})};
    cppurses::Color_select& color_select_background{
        color_select_stack.make_page<cppurses::Color_select>(
            cppurses::Glyph_string{"Background", cppurses::Attribute::Bold})};

    Attribute_box& attribute_box{this->make_child<Attribute_box>()};

    cppurses::Status_bar& show_glyph{
        this->make_child<cppurses::Status_bar>("x")};

    cppurses::Blank_height& space2{this->make_child<cppurses::Blank_height>(1)};
    Options_stack& options_box{this->make_child<Options_stack>()};
};

}  // namespace glyph_paint
}  // namespace demos
#endif  // DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
