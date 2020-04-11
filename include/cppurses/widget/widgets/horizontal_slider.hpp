#ifndef CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SLIDER_HPP
#define CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SLIDER_HPP
#include <cstddef>

#include <signals/signals.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

class Horizontal_slider : public Widget {
   public:
    sig::Signal<void(float)> percent_changed;
    sig::Signal<void()> scrolled_up;
    sig::Signal<void()> scrolled_down;

   public:
    Horizontal_slider()
    {
        using namespace pipe;
        *this | fixed_height(1) | strong_focus() |
            wallpaper(L' ' | background(Color::Light_gray));
    }

    void set_percent(float percent);

    auto percent() const -> float { return percent_progress_; }

   protected:
    auto paint_event() -> bool override
    {
        auto const x = percent_to_position(percent_progress_);
        Painter{*this}.put(indicator_, x, 0);
        return Widget::paint_event();
    }

    auto mouse_press_event(Mouse::State const& mouse) -> bool override;

    auto mouse_wheel_event(Mouse::State const& mouse) -> bool override;

    auto key_press_event(Key::State const& keyboard) -> bool override;

   private:
    Glyph indicator_        = L'░';
    float percent_progress_ = 0.0;

   private:
    auto position_to_percent(std::size_t position) -> float;
    auto percent_to_position(float percent) -> std::size_t;
};

namespace slot {

auto set_percent(Horizontal_slider& s) -> sig::Slot<void(float)>;

auto set_percent(Horizontal_slider& s, float percent) -> sig::Slot<void(void)>;

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SLIDER_HPP
