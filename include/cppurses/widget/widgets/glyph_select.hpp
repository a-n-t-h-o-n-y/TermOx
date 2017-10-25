#ifndef CPPURSES_WIDGET_WIDGETS_GLYPH_SELECT_HPP
#define CPPURSES_WIDGET_WIDGETS_GLYPH_SELECT_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widgets/glyph_select_stack.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/slider.hpp>

#include <signals/signal.hpp>

namespace cppurses {

class Glyph_select : public Vertical_layout {
   public:
    Glyph_select(Glyph_string symbols = "");

    void set_symbols(Glyph_string symbols);

    // Signals
    sig::Signal<void(Glyph)> glyph_selected;

   private:
    Glyph_select_stack& glyph_stack{this->make_child<Glyph_select_stack>()};
    Horizontal_layout& bottom_row{this->make_child<Horizontal_layout>()};
    Slider& slider{bottom_row.make_child<Slider>()};
    Push_button& left_btn{bottom_row.make_child<Push_button>("<")};
    Push_button& right_btn{bottom_row.make_child<Push_button>(">")};

    std::size_t current_index();
    void set_stack_index_by_percent(float percent);
    void flip_page_forward();
    void flip_page_backward();
};

}  // namespace cppurses

#endif  // CPPURSES_WIDGET_WIDGETS_GLYPH_SELECT_HPP
