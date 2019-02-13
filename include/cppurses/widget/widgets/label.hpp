#ifndef CPPURSES_WIDGET_WIDGETS_LABEL_HPP
#define CPPURSES_WIDGET_WIDGETS_LABEL_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace cppurses {

/// A simple Text_display with fixed height 1 and no word wrapping.
/** Common use case of Text_display where you only intend to display a word or
 *  two and don't want to mess with the boilerplate every time. */
struct Label : Text_display {
    explicit Label(Glyph_string text = "");
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_LABEL_HPP
