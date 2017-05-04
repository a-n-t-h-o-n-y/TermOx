#ifndef PUSH_BUTTON_HPP
#define PUSH_BUTTON_HPP

#include "widget_module/widget.hpp"
#include "system_module/events/mouse_event.hpp"
#include "painter_module/painter.hpp"

#include <aml/signals/signal.hpp>

namespace twf {

class Push_button : public Widget {
   public:
    Push_button(Glyph_string name = "");
    bool mouse_press_event(const Mouse_event& event) override;
    bool paint_event(const Paint_event& event) override;

    // Signals
    sig::Signal<void()> clicked;

   private:
    Glyph_string title_;
};
}  // namespace twf
#endif  // PUSH_BUTTON_HPP
