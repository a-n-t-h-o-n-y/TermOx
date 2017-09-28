#ifndef WIDGET_WIDGETS_TITLEBAR_HPP
#define WIDGET_WIDGETS_TITLEBAR_HPP
#include "painter/glyph_string.hpp"
#include "widget/layouts/horizontal_layout.hpp"
#include <signals/slot.hpp>

namespace cppurses {
class Push_button;

class Titlebar : public Horizontal_layout {
   public:
    explicit Titlebar(const Glyph_string& title = "");
    void set_title(Glyph_string title);

   private:
    Push_button& title_;
    Push_button& exit_button_;
};

namespace slot {

sig::Slot<void()> set_title(Titlebar& tb, const Glyph_string& title);
sig::Slot<void(Glyph_string)> set_title(Titlebar& tb);

}  // namespace slot

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_TITLEBAR_HPP
