#ifndef DEMOS_GLYPH_PAINT_ATTRIBUTE_BOX_HPP
#define DEMOS_GLYPH_PAINT_ATTRIBUTE_BOX_HPP
#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>

namespace demos {
namespace glyph_paint {

class Attribute_box : public cppurses::layout::Vertical<> {
    using Checkbox = cppurses::Checkbox;
    using Gs       = cppurses::Glyph_string;
    struct Top_row : cppurses::layout::Horizontal<Checkbox> {
        Checkbox& bold_box{
            this->make_child(Gs{"Bold", cppurses::Attribute::Bold})};
        Checkbox& dim_box{
            this->make_child(Gs{"Dim", cppurses::Attribute::Dim})};
    };

   public:
    Attribute_box() { this->height_policy.maximum(4); }

    Top_row& top_row{this->make_child<Top_row>()};

    Checkbox& inverse_box{this->make_child<Checkbox>(
        Gs{"Inverse", cppurses::Attribute::Inverse})};

    Checkbox& italic_box{
        this->make_child<Checkbox>(Gs{"Italic", cppurses::Attribute::Italic})};

    Checkbox& underline_box{this->make_child<Checkbox>(
        Gs{"Underline", cppurses::Attribute::Underline})};
};

}  // namespace glyph_paint
}  // namespace demos
#endif  // DEMOS_GLYPH_PAINT_ATTRIBUTE_BOX_HPP
