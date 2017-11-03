#ifndef WIDGET_WIDGETS_PUSH_BUTTON_HPP
#define WIDGET_WIDGETS_PUSH_BUTTON_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/label.hpp>

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
                           Point global,
                           Point local,
                           std::uint8_t device_id) override;
};

namespace slot {

sig::Slot<void()> click(Push_button& pb);

}  // namespace slot
}  // namespace cppurses
#endif  // WIDGET_WIDGETS_PUSH_BUTTON_HPP
