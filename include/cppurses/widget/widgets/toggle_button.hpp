#ifndef CPPURSES_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
#define CPPURSES_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
#include <signals/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/widget_stack.hpp>

namespace cppurses {

struct Toggle_button : Widget_stack {
    Toggle_button(Glyph_string first_label, Glyph_string second_label);

    Push_button& first_btn;
    Push_button& second_btn;

    sig::Signal<void()>& first_clicked{first_btn.clicked};
    sig::Signal<void()>& second_clicked{second_btn.clicked};
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
