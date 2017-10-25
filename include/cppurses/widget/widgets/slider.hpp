#ifndef CPPURSES_WIDGET_WIDGETS_SLIDER_HPP
#define CPPURSES_WIDGET_WIDGETS_SLIDER_HPP
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/widget/widget.hpp>

#include <signals/signals.hpp>

#include <cstddef>
#include <cstdint>

namespace cppurses {

class Slider : public Widget {
   public:
    Slider();
    void set_percent(float percent);
    float percent() const;

    // Signals
    sig::Signal<void(float)> percent_changed;
    sig::Signal<void()> scrolled_up;
    sig::Signal<void()> scrolled_down;

   protected:
    bool paint_event() override;
    bool mouse_press_event(Mouse_button button,
                           std::size_t global_x,
                           std::size_t global_y,
                           std::size_t local_x,
                           std::size_t local_y,
                           std::uint8_t device_id) override;
    bool key_press_event(Key key, char symbol) override;

   private:
    Glyph indicator_{"│"};
    float percent_progress_{0.0};

    float position_to_percent(std::size_t position);
    std::size_t percent_to_position(float percent);
};

namespace slot {

sig::Slot<void(float)> set_percent(Slider& s);

sig::Slot<void()> set_percent(Slider& s, float percent);

}  // namespace slot

}  // namespace cppurses

#endif  // CPPURSES_WIDGET_WIDGETS_SLIDER_HPP
