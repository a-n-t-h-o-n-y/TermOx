#ifndef CPPURSES_WIDGET_WIDGETS_TITLEBAR_HPP
#define CPPURSES_WIDGET_WIDGETS_TITLEBAR_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>

namespace cppurses {
class Push_button;
struct Label;

/// Provides a centered title and flush-right exit button.
/** The exit button will call System::quit() when pressed. Titlebar is fixed at
 *  a height of 1. */
struct Titlebar : public layout::Horizontal {
    /// Construct a Titlebar with centered \p title.
    explicit Titlebar(Glyph_string title_ = "");

    /// The centered title Widget.
    Label& title;

    /// The exit button Widget.
    Push_button& exit_button;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TITLEBAR_HPP
