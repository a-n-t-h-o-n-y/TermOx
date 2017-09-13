#ifndef WIDGET_WIDGETS_TITLEBAR_HPP
#define WIDGET_WIDGETS_TITLEBAR_HPP

#include "painter/glyph_string.hpp"
#include "widget/layouts/horizontal_layout.hpp"

namespace cppurses {
class Push_button;

class Titlebar : public Horizontal_layout {
   public:
    explicit Titlebar(const Glyph_string& title = "");

   private:
    Push_button& title_;
    Push_button& exit_button_;
};

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_TITLEBAR_HPP
