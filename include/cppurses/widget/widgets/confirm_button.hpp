#ifndef CPPURSES_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP
#define CPPURSES_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP
#include <signals/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

/// Provides a button with a double check confirmation interface.
struct Confirm_button : public layout::Stack {
    explicit Confirm_button(Glyph_string title,
                            Glyph_string confirm_text = "Confirm");

    struct Confirm_screen;
    Push_button& title_btn_;
    Confirm_screen& confirm_screen_;

    // Signals
    sig::Signal<void()> clicked;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_CONFIRM_BUTTON_HPP
