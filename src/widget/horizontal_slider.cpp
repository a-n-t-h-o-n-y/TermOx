#include <cppurses/widget/widgets/horizontal_slider.hpp>

#include <cmath>
#include <cstddef>

#include <signals/signals.hpp>

#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>

namespace cppurses {

void Horizontal_slider::set_percent(float percent)
{
    if (percent < 0.0)
        percent_progress_ = 0.0;
    else if (percent > 1.0)
        percent_progress_ = 1.0;
    else
        percent_progress_ = percent;
    percent_changed(percent_progress_);
    this->update();
}

auto Horizontal_slider::mouse_press_event(Mouse::State const& mouse) -> bool
{
    switch (mouse.button) {
        case Mouse::Button::Left:
            this->set_percent(position_to_percent(mouse.local.x));
            break;
        case Mouse::Button::ScrollUp: scrolled_up(); break;
        case Mouse::Button::ScrollDown: scrolled_down(); break;
        default: break;
    }
    return Widget::mouse_press_event(mouse);
}

auto Horizontal_slider::key_press_event(Key::State const& keyboard) -> bool
{
    auto const current_position = percent_to_position(percent_progress_);
    switch (keyboard.key) {
        case Key::Arrow_right:
            this->set_percent(position_to_percent(current_position + 1));
            break;
        case Key::Arrow_left:
            if (current_position != 0)
                this->set_percent(position_to_percent(current_position - 1));
            break;
        default: break;
    }
    return Widget::key_press_event(keyboard);
}

auto Horizontal_slider::position_to_percent(std::size_t position) -> float
{
    auto const width = this->width();
    if (width < 2)
        return 0.f;
    if (position >= (width - 1))
        return 1.f;
    return static_cast<float>(position) / (width - 1);
}

auto Horizontal_slider::percent_to_position(float percent) -> std::size_t
{
    auto const width = this->width();
    return width == 0 ? 0 : std::round(percent * (width - 1));
}

namespace slot {

auto set_percent(Horizontal_slider& s) -> sig::Slot<void(float)>
{
    auto slot =
        sig::Slot<void(float)>{[&s](float percent) { s.set_percent(percent); }};
    slot.track(s.destroyed);
    return slot;
}

auto set_percent(Horizontal_slider& s, float percent) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&s, percent] { s.set_percent(percent); }};
    slot.track(s.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
