#ifndef CPPURSES_DEMOS_GLYPH_PAINT_GLYPH_SELECT_HPP
#define CPPURSES_DEMOS_GLYPH_PAINT_GLYPH_SELECT_HPP
#include <cstddef>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/horizontal_slider.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

#include "glyph_select_stack.hpp"

namespace demos {
namespace glyph_paint {

/// Holds pages of Glyphs that can be cycled through and selected.
/** When a Glyph is selected a Signal is emitted with that Glyph sent along. */
class Glyph_select : public cppurses::layout::Vertical {
   public:
    /// Construct with \p symbols has contents.
    explicit Glyph_select(cppurses::Glyph_string symbols = "");

    /// Set \p symbols as the contents.
    void set_symbols(cppurses::Glyph_string symbols);

    /// Emitted when a Glyph is clicked, sending along the glyph.
    sig::Signal<void(cppurses::Glyph)> glyph_selected;

   private:
    Glyph_select_stack& glyph_stack{this->make_child<Glyph_select_stack>()};
    cppurses::layout::Horizontal& bottom_row{
        this->make_child<cppurses::layout::Horizontal>()};
    cppurses::Push_button& left_btn{
        bottom_row.make_child<cppurses::Push_button>(
            cppurses::Glyph{L'⏴', foreground(cppurses::Color::Black)})};
    cppurses::Horizontal_slider& slider{
        bottom_row.make_child<cppurses::Horizontal_slider>()};
    cppurses::Push_button& right_btn{
        bottom_row.make_child<cppurses::Push_button>(
            cppurses::Glyph{L'⏵', foreground(cppurses::Color::Black)})};

    std::size_t current_index();
    void flip_page_forward();
    void flip_page_backward();
};

}  // namespace glyph_paint
}  // namespace demos
#endif  // CPPURSES_DEMOS_GLYPH_PAINT_GLYPH_SELECT_HPP
