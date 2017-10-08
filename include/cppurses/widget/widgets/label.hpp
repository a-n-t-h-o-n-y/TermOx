#ifndef WIDGET_WIDGETS_LABEL_HPP
#define WIDGET_WIDGETS_LABEL_HPP
#include "painter/glyph_string.hpp"
#include "widget/widget.hpp"

namespace cppurses {

enum class Alignment { Left, Center, Right };

class Label : public Widget {
   public:
    Label(Glyph_string text = "");

    void set_text(Glyph_string text);
    void set_alignment(Alignment type);

   protected:
    bool paint_event() override;

   private:
    Glyph_string text_;
    Alignment alignment_{Alignment::Left};
};

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_LABEL_HPP
