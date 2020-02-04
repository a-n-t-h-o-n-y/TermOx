#ifndef CPPURSES_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP
#define CPPURSES_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP
#include <signals/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/stack.hpp>

namespace cppurses {
class Push_button;

/// A Push_button with a confirm screen after the initial click.
/** Clicked signal is only emitted after confirmation. */
struct Confirm_button : public layout::Stack<> {
    /// Front facing button.
    Push_button& main_btn;

    struct Confirm_screen;
    /// Confirmation button.
    Confirm_screen& confirm_screen;

    /// Emitted once confirmation has been made.
    sig::Signal<void()> clicked;

    /// Construct a Push_button with \p label and corresponding \p confirm_text.
    explicit Confirm_button(Glyph_string label,
                            Glyph_string confirm_text = "Confirm");
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP
