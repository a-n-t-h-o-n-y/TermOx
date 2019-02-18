#ifndef CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SLIDER_HPP
#define CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SLIDER_HPP
#include <cstddef>
#include <cstdint>

#include <signals/signals.hpp>

#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

class Horizontal_slider : public Widget {
   public:
    Horizontal_slider();
    void set_percent(float percent);
    float percent() const;

    // Signals
    sig::Signal<void(float)> percent_changed;
    sig::Signal<void()> scrolled_up;
    sig::Signal<void()> scrolled_down;

   protected:
    bool paint_event() override;
    bool mouse_press_event(const Mouse::State& mouse) override;
    bool key_press_event(const Key::State& keyboard) override;

   private:
    Glyph indicator_{L'░'};
    float percent_progress_{0.0};

    float position_to_percent(std::size_t position);
    std::size_t percent_to_position(float percent);
};

namespace slot {

sig::Slot<void(float)> set_percent(Horizontal_slider& s);

sig::Slot<void()> set_percent(Horizontal_slider& s, float percent);

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SLIDER_HPP
