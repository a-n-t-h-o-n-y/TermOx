#ifndef DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
#define DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
#include "options_box.hpp"
#include "populated_glyph_stack.hpp"
#include "trait_box.hpp"

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/color_select.hpp>
#include <cppurses/widget/widgets/cycle_stack.hpp>
#include <cppurses/widget/widgets/status_bar.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace demos::glyph_paint {

class Side_pane : public cppurses::layout::Vertical<> {
   private:
    struct Color_pages : cppurses::Cycle_stack<cppurses::Color_select> {
        Color_pages() { this->height_policy.fixed(3); }

        cppurses::Color_select& foreground =
            this->make_page(L"Foreground" | cppurses::Trait::Bold);
        cppurses::Color_select& background =
            this->make_page(L"Background" | cppurses::Trait::Bold);
    };

   public:
    Side_pane()
    {
        this->width_policy.fixed(16);
        space1.set_wallpaper(L'─');
        space2.set_wallpaper(L'─');

        glyph_select.height_policy.preferred(6);

        show_glyph.height_policy.fixed(1);
        show_glyph.set_alignment(cppurses::Align::Center);
    }

   public:
    Populated_glyph_stack& glyph_select =
        this->make_child<Populated_glyph_stack>();

    Widget& space1 = this->make_child() | cppurses::pipe::fixed_height(1);

    Color_pages& color_pages = this->make_child<Color_pages>();

    Trait_box& trait_box = this->make_child<Trait_box>();

    cppurses::Status_bar& show_glyph =
        this->make_child<cppurses::Status_bar>("x");

    Widget& space2 = this->make_child() | cppurses::pipe::fixed_height(1);

    Options_stack& options_box = this->make_child<Options_stack>();
};

}  // namespace demos::glyph_paint
#endif  // DEMOS_GLYPH_PAINT_SIDE_PANE_HPP
