#ifndef DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
#define DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/trait.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/color_select.hpp>
#include <termox/widget/widgets/cycle_stack.hpp>
#include <termox/widget/widgets/status_bar.hpp>
#include <termox/widget/widgets/text_display.hpp>

#include "glyph_selector.hpp"
#include "options_box.hpp"
#include "trait_box.hpp"

namespace paint {

class Side_pane : public ox::layout::Vertical<> {
   private:
    struct Color_pages : ox::Cycle_stack<ox::Color_select> {
        Color_pages() { height_policy.fixed(3); }

        ox::Color_select& foreground =
            make_page(L"Foreground" | ox::Trait::Bold);
        ox::Color_select& background =
            make_page(L"Background" | ox::Trait::Bold);
    };

   public:
    Side_pane()
    {
        using namespace ox::pipe;

        *this | fixed_width(16);
        space1 | wallpaper(L'─');
        space2 | wallpaper(L'─');
        glyph_selector | preferred_height(6);
        show_glyph | fixed_height(1) | align_center();
    }

   public:
    Glyph_selector& glyph_selector = append_child(populated_glyph_selector());
    Widget& space1                 = make_child() | ox::pipe::fixed_height(1);
    Color_pages& color_pages       = make_child<Color_pages>();
    Trait_box& trait_box           = make_child<Trait_box>();
    ox::Status_bar& show_glyph     = make_child<ox::Status_bar>("x");
    Widget& space2                 = make_child() | ox::pipe::fixed_height(1);
    Options_stack& options_box     = make_child<Options_stack>();
};

}  // namespace paint
#endif  // DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
