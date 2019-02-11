#ifndef CPPURSES_WIDGET_WIDGETS_GLYPH_SELECT_HPP
#define CPPURSES_WIDGET_WIDGETS_GLYPH_SELECT_HPP
#include <cstddef>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/glyph_select_stack.hpp>
#include <cppurses/widget/widgets/horizontal_slider.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

/// Holds pages of Glyphs that can be cycled through and selected.
/** When a Glyph is selected a Signal is emitted with that Glyph sent along. */
class Glyph_select : public layout::Vertical {
   public:
    explicit Glyph_select(Glyph_string symbols = "");

    void set_symbols(Glyph_string symbols);

    // Signals
    sig::Signal<void(Glyph)> glyph_selected;

   private:
    Glyph_select_stack& glyph_stack{this->make_child<Glyph_select_stack>()};
    layout::Horizontal& bottom_row{this->make_child<layout::Horizontal>()};
    Push_button& left_btn{bottom_row.make_child<Push_button>(
        Glyph{L'⏴', foreground(Color::Black)})};
    Horizontal_slider& slider{bottom_row.make_child<Horizontal_slider>()};
    Push_button& right_btn{bottom_row.make_child<Push_button>(
        Glyph{L'⏵', foreground(Color::Black)})};

    std::size_t current_index();
    void flip_page_forward();
    void flip_page_backward();
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_GLYPH_SELECT_HPP
