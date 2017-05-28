#ifndef WIDGET_MODULE_WIDGETS_PUSH_BUTTON_HPP
#define WIDGET_MODULE_WIDGETS_PUSH_BUTTON_HPP

#include "painter_module/glyph_string.hpp"
#include "system_module/events/mouse_event.hpp"
#include "system_module/events/paint_event.hpp"
#include "widget_module/widget.hpp"
#include <aml/signals/signals.hpp>

namespace cppurses {

class Push_button : public Widget {
   public:
    explicit Push_button(Glyph_string name = "");
    bool mouse_press_event(const Mouse_event& event) override;
    bool paint_event(const Paint_event& event) override;

    // Slots
    sig::Slot<void()> click;

    // Signals
    sig::Signal<void()> clicked;

   private:
    Glyph_string title_;
};
}  // namespace cppurses
#endif  // WIDGET_MODULE_WIDGETS_PUSH_BUTTON_HPP
