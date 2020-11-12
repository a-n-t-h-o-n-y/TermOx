#ifndef DEMOS_GLYPH_PAINT_TRAIT_BOX_HPP
#define DEMOS_GLYPH_PAINT_TRAIT_BOX_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>
#include <cppurses/widget/widgets/label.hpp>

namespace demos {
namespace glyph_paint {

class Trait_box : public cppurses::layout::Vertical<> {
    using Checkbox = cppurses::Labeled_checkbox;
    using Gs       = cppurses::Glyph_string;
    struct Top_row : cppurses::layout::Horizontal<Checkbox> {
        Checkbox& bold_box{this->make_child(Gs{"Bold", cppurses::Trait::Bold})};
        Checkbox& dim_box{this->make_child(Gs{"Dim", cppurses::Trait::Dim})};
    };

   public:
    Trait_box() { this->height_policy.fixed(4); }

    Top_row& top_row = this->make_child<Top_row>();
    Checkbox& inverse_box =
        this->make_child<Checkbox>(Gs{"Inverse", cppurses::Trait::Inverse});
    Checkbox& italic_box =
        this->make_child<Checkbox>(Gs{"Italic", cppurses::Trait::Italic});
    Checkbox& underline_box =
        this->make_child<Checkbox>(Gs{"Underline", cppurses::Trait::Underline});
};

}  // namespace glyph_paint
}  // namespace demos
#endif  // DEMOS_GLYPH_PAINT_TRAIT_BOX_HPP
