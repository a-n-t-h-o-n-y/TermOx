#ifndef DEMOS_GLYPH_PAINT_ATTRIBUTE_BOX_HPP
#define DEMOS_GLYPH_PAINT_ATTRIBUTE_BOX_HPP
#include <cppurses/cppurses.hpp>

using namespace cppurses;

namespace demos {
namespace glyph_paint {

struct Attribute_box : public Vertical_layout {
    Attribute_box();
    Horizontal_layout& top_row{this->make_child<Horizontal_layout>()};
    Checkbox& bold_box{
        top_row.make_child<Checkbox>(Glyph_string{"Bold", Attribute::Bold})};
    Checkbox& dim_box{
        top_row.make_child<Checkbox>(Glyph_string{"Dim", Attribute::Dim})};
    Checkbox& inverse_box{this->make_child<Checkbox>(
        Glyph_string{"Inverse", Attribute::Inverse})};
    Checkbox& italic_box{
        this->make_child<Checkbox>(Glyph_string{"Italic", Attribute::Italic})};
    Checkbox& underline_box{this->make_child<Checkbox>(
        Glyph_string{"Underline", Attribute::Underline})};
};

}  // namespace glyph_paint
}  // namespace demos

#endif  // DEMOS_GLYPH_PAINT_ATTRIBUTE_BOX_HPP
