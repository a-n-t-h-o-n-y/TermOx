#ifndef WIDGET_WIDGETS_LABEL_HPP
#define WIDGET_WIDGETS_LABEL_HPP
#include "painter/glyph_string.hpp"
#include "widget/widgets/text_display.hpp"

namespace cppurses {

class Label : public Text_display {
   public:
    explicit Label(Glyph_string text = "");
};

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_LABEL_HPP
