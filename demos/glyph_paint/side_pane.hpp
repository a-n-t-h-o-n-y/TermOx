#ifndef DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
#define DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/color_select.hpp>
#include <cppurses/widget/widgets/cycle_stack.hpp>
#include <cppurses/widget/widgets/status_bar.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

#include "glyph_selector.hpp"
#include "options_box.hpp"
#include "trait_box.hpp"

namespace paint {

class Side_pane : public cppurses::layout::Vertical<> {
   private:
    struct Color_pages : cppurses::Cycle_stack<cppurses::Color_select> {
        Color_pages() { height_policy.fixed(3); }

        cppurses::Color_select& foreground =
            make_page(L"Foreground" | cppurses::Trait::Bold);
        cppurses::Color_select& background =
            make_page(L"Background" | cppurses::Trait::Bold);
    };

   public:
    Side_pane()
    {
        using namespace cppurses::pipe;

        *this | fixed_width(16);
        space1 | wallpaper(L'─');
        space2 | wallpaper(L'─');
        glyph_selector | preferred_height(6);
        show_glyph | fixed_height(1) | align_center();
    }

   public:
    Glyph_selector& glyph_selector = append_child(populated_glyph_selector());
    Widget& space1           = make_child() | cppurses::pipe::fixed_height(1);
    Color_pages& color_pages = make_child<Color_pages>();
    Trait_box& trait_box     = make_child<Trait_box>();
    cppurses::Status_bar& show_glyph = make_child<cppurses::Status_bar>("x");
    Widget& space2             = make_child() | cppurses::pipe::fixed_height(1);
    Options_stack& options_box = make_child<Options_stack>();
};

}  // namespace paint
#endif  // DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
