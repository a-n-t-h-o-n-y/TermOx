#ifndef DEMOS_GLYPH_PAINT_TRAIT_BOX_HPP
#define DEMOS_GLYPH_PAINT_TRAIT_BOX_HPP
#include <termox/painter/glyph_string.hpp>
#include <termox/painter/trait.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/widgets/checkbox.hpp>
#include <termox/widget/widgets/label.hpp>

namespace paint {

class Trait_box : public ox::layout::Vertical<> {
   private:
    using Checkbox = ox::HCheckbox_label;

    struct Top_row : ox::layout::Horizontal<Checkbox> {
        Checkbox& bold_box = make_child({L"Bold" | ox::Trait::Bold});
        Checkbox& dim_box  = make_child({L"Dim" | ox::Trait::Dim});
    };

   public:
    Top_row& top_row = make_child<Top_row>();
    Checkbox& inverse_box =
        make_child<Checkbox>({L"Inverse" | ox::Trait::Inverse});
    Checkbox& italic_box =
        make_child<Checkbox>({L"Italic" | ox::Trait::Italic});
    Checkbox& underline_box =
        make_child<Checkbox>({L"Underline" | ox::Trait::Underline});

   public:
    Trait_box() { this->height_policy.fixed(4); }
};

}  // namespace paint
#endif  // DEMOS_GLYPH_PAINT_TRAIT_BOX_HPP
