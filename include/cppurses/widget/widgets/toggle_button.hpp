#ifndef CPPURSES_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
#define CPPURSES_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
#include <signals/signal.hpp>

#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {
class Glyph_string;

/// A button with two alternating states.
/** The top state is active first. State is changed to the non-active state on
 *  press of the currently active button. */
struct Toggle_button : layout::Stack {
    /// Construct with corresponding labels.
    Toggle_button(Glyph_string top_label, Glyph_string bottom_label);

    Push_button& top;
    Push_button& bottom;

    sig::Signal<void()>& top_clicked{top.clicked};
    sig::Signal<void()>& bottom_clicked{bottom.clicked};
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TOGGLE_BUTTON_HPP
