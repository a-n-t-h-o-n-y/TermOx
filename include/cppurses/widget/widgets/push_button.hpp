#ifndef WIDGET_WIDGETS_PUSH_BUTTON_HPP
#define WIDGET_WIDGETS_PUSH_BUTTON_HPP

#include <signals/signals.hpp>
#include <utility>
#include "painter/glyph_string.hpp"
#include "system/events/mouse_event.hpp"
#include "system/events/paint_event.hpp"
#include "widget/widget.hpp"

namespace cppurses {

class Push_button : public Widget {
   public:
    explicit Push_button(Glyph_string name = "");

    void set_text(Glyph_string text);

    // Signals
    sig::Signal<void()> clicked;
    sig::Signal<void(Push_button*)> clicked_w_ref;

   protected:
    bool mouse_press_event(Mouse_button button,
                           std::size_t global_x,
                           std::size_t global_y,
                           std::size_t local_x,
                           std::size_t local_y,
                           std::uint8_t device_id) override;
    bool paint_event() override;

   private:
    Glyph_string title_;
};

namespace slot {

sig::Slot<void()> click(Push_button& pb);

}  // namespace slot
}  // namespace cppurses
#endif  // WIDGET_WIDGETS_PUSH_BUTTON_HPP
