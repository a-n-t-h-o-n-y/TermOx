#ifndef WIDGET_WIDGETS_PUSH_BUTTON_HPP
#define WIDGET_WIDGETS_PUSH_BUTTON_HPP

#include "painter/glyph_string.hpp"
#include "system/events/mouse_event.hpp"
#include "system/events/paint_event.hpp"
#include "widget/widget.hpp"
#include <signals/signals.hpp>
#include <utility>

namespace cppurses {

class Push_button : public Widget {
   public:
    explicit Push_button(Glyph_string name = "");
    bool mouse_press_event(Mouse_button button,
                           std::size_t global_x,
                           std::size_t global_y,
                           std::size_t local_x,
                           std::size_t local_y,
                           std::uint8_t device_id) override;
    bool paint_event() override;

    // Slots
    sig::Slot<void()> click;

    // Signals
    sig::Signal<void()> clicked;
    sig::Signal<void(Push_button*)> clicked_w_ref;

    void set_text(Glyph_string text) {
        title_ = std::move(text);
        this->update();
    }

   private:
    Glyph_string title_;
};
}  // namespace cppurses
#endif  // WIDGET_WIDGETS_PUSH_BUTTON_HPP
