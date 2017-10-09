#ifndef WIDGET_WIDGETS_PUSH_BUTTON_HPP
#define WIDGET_WIDGETS_PUSH_BUTTON_HPP
#include "painter/glyph_string.hpp"
#include "system/mouse_button.hpp"
#include "widget/widget.hpp"
#include "widget/widgets/label.hpp"

#include <signals/signals.hpp>

#include <cstddef>
#include <cstdint>

namespace cppurses {

class Push_button : public Label {
   public:
    explicit Push_button(Glyph_string name = "");

    // Signals
    sig::Signal<void()> clicked;

   protected:
    bool mouse_press_event(Mouse_button button,
                           std::size_t global_x,
                           std::size_t global_y,
                           std::size_t local_x,
                           std::size_t local_y,
                           std::uint8_t device_id) override;
};

namespace slot {

sig::Slot<void()> click(Push_button& pb);

}  // namespace slot
}  // namespace cppurses
#endif  // WIDGET_WIDGETS_PUSH_BUTTON_HPP
