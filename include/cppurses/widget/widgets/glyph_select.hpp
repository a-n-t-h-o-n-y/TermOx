#ifndef CPPURSES_WIDGET_WIDGETS_GLYPH_SELECT_HPP
#define CPPURSES_WIDGET_WIDGETS_GLYPH_SELECT_HPP
#include <cstddef>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widgets/glyph_select_stack.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/slider.hpp>

namespace cppurses {

class Glyph_select : public Vertical_layout {
   public:
    explicit Glyph_select(Glyph_string symbols = "");

    void set_symbols(Glyph_string symbols);

    // Signals
    sig::Signal<void(Glyph)> glyph_selected;

   private:
    Glyph_select_stack& glyph_stack{this->make_child<Glyph_select_stack>()};
    Horizontal_layout& bottom_row{this->make_child<Horizontal_layout>()};
    Push_button& left_btn{bottom_row.make_child<Push_button>(
        Glyph{L'⏴', foreground(Color::Black)})};
    Slider& slider{bottom_row.make_child<Slider>()};
    Push_button& right_btn{bottom_row.make_child<Push_button>(
        Glyph{L'⏵', foreground(Color::Black)})};

    std::size_t current_index();
    void flip_page_forward();
    void flip_page_backward();
};

}  // namespace cppurses

#endif  // CPPURSES_WIDGET_WIDGETS_GLYPH_SELECT_HPP
