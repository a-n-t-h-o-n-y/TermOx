#ifndef DEMOS_GLYPH_PAINT_TRAIT_BOX_HPP
#define DEMOS_GLYPH_PAINT_TRAIT_BOX_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>
#include <cppurses/widget/widgets/label.hpp>

namespace paint {

class Trait_box : public cppurses::layout::Vertical<> {
   private:
    using Checkbox = cppurses::Labeled_checkbox;

    struct Top_row : cppurses::layout::Horizontal<Checkbox> {
        Checkbox& bold_box = make_child(L"Bold" | cppurses::Trait::Bold);
        Checkbox& dim_box  = make_child(L"Dim" | cppurses::Trait::Dim);
    };

   public:
    Top_row& top_row = make_child<Top_row>();
    Checkbox& inverse_box =
        make_child<Checkbox>(L"Inverse" | cppurses::Trait::Inverse);
    Checkbox& italic_box =
        make_child<Checkbox>(L"Italic" | cppurses::Trait::Italic);
    Checkbox& underline_box =
        make_child<Checkbox>(L"Underline" | cppurses::Trait::Underline);

   public:
    Trait_box() { this->height_policy.fixed(4); }
};

}  // namespace paint
#endif  // DEMOS_GLYPH_PAINT_TRAIT_BOX_HPP
