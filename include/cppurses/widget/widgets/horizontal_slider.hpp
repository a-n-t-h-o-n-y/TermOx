#ifndef CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SLIDER_HPP
#define CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SLIDER_HPP
#include <cstddef>

#include <signals_light/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/mouse.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

class Horizontal_slider : public Widget {
   public:
    sl::Signal<void(float)> percent_changed;
    sl::Signal<void()> scrolled_up;
    sl::Signal<void()> scrolled_down;

   public:
    Horizontal_slider()
    {
        using namespace pipe;
        *this | fixed_height(1) | strong_focus() |
            wallpaper(L' ' | bg(Color::Light_gray));
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

    auto mouse_press_event(Mouse const& m) -> bool override;

    auto mouse_wheel_event(Mouse const& m) -> bool override;

    auto key_press_event(Key k) -> bool override;

   private:
    Glyph indicator_        = L'░';
    float percent_progress_ = 0.0;

   private:
    auto position_to_percent(std::size_t position) -> float;
    auto percent_to_position(float percent) -> std::size_t;
};

/// Helper function to create an instance.
template <typename... Args>
auto horizontal_slider(Args&&... args) -> std::unique_ptr<Horizontal_slider>
{
    return std::make_unique<Horizontal_slider>(std::forward<Args>(args)...);
}

namespace slot {

auto set_percent(Horizontal_slider& s) -> sl::Slot<void(float)>;

auto set_percent(Horizontal_slider& s, float percent) -> sl::Slot<void(void)>;

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SLIDER_HPP
