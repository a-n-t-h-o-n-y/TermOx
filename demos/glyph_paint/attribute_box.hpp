#ifndef DEMOS_GLYPH_PAINT_ATTRIBUTE_BOX_HPP
#define DEMOS_GLYPH_PAINT_ATTRIBUTE_BOX_HPP
#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>

namespace demos {
namespace glyph_paint {

struct Attribute_box : public cppurses::Vertical_layout {
    Attribute_box();
    cppurses::Horizontal_layout& top_row{
        this->make_child<cppurses::Horizontal_layout>()};
    cppurses::Checkbox& bold_box{top_row.make_child<cppurses::Checkbox>(
        cppurses::Glyph_string{"Bold", cppurses::Attribute::Bold})};
    cppurses::Checkbox& dim_box{top_row.make_child<cppurses::Checkbox>(
        cppurses::Glyph_string{"Dim", cppurses::Attribute::Dim})};
    cppurses::Checkbox& inverse_box{this->make_child<cppurses::Checkbox>(
        cppurses::Glyph_string{"Inverse", cppurses::Attribute::Inverse})};
    cppurses::Checkbox& italic_box{this->make_child<cppurses::Checkbox>(
        cppurses::Glyph_string{"Italic", cppurses::Attribute::Italic})};
    cppurses::Checkbox& underline_box{this->make_child<cppurses::Checkbox>(
        cppurses::Glyph_string{"Underline", cppurses::Attribute::Underline})};
};

}  // namespace glyph_paint
}  // namespace demos
#endif  // DEMOS_GLYPH_PAINT_ATTRIBUTE_BOX_HPP
