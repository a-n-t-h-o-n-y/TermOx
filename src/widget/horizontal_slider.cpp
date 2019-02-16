#include <cppurses/widget/widgets/horizontal_slider.hpp>

#include <cmath>

#include <signals/signals.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/keyboard_data.hpp>

namespace cppurses {

Horizontal_slider::Horizontal_slider() {
    this->height_policy.fixed(1);
    this->focus_policy = Focus_policy::Strong;
    this->wallpaper = Glyph{L' ', background(Color::Light_gray)};
}

void Horizontal_slider::set_percent(float percent) {
    if (percent < 0.0) {
        percent_progress_ = 0.0;
    } else if (percent > 1.0) {
        percent_progress_ = 1.0;
    } else {
        percent_progress_ = percent;
    }
    percent_changed(percent_progress_);
    this->update();
}

float Horizontal_slider::percent() const {
    return percent_progress_;
}

bool Horizontal_slider::paint_event() {
    std::size_t x{percent_to_position(percent_progress_)};
    Painter p{*this};
    p.put(indicator_, x, 0);
    return Widget::paint_event();
}

bool Horizontal_slider::mouse_press_event(const Mouse::State& mouse) {
    if (mouse.button == Mouse::Button::Left) {
        this->set_percent(position_to_percent(mouse.local.x));
    } else if (mouse.button == Mouse::Button::ScrollUp) {
        scrolled_up();
    } else if (mouse.button == Mouse::Button::ScrollDown) {
        scrolled_down();
    }
    return Widget::mouse_press_event(mouse);
}

bool Horizontal_slider::key_press_event(const Keyboard_data& keyboard) {
    std::size_t current_position = percent_to_position(percent_progress_);
    if (keyboard.key == Key::Arrow_right) {
        this->set_percent(position_to_percent(current_position + 1));
    } else if (keyboard.key == Key::Arrow_left) {
        if (current_position != 0) {
            this->set_percent(position_to_percent(current_position - 1));
        }
    }
    return Widget::key_press_event(keyboard);
}

float Horizontal_slider::position_to_percent(std::size_t position) {
    std::size_t w = this->width();
    if (w < 2) {
        return 0.0;
    }
    --w;
    if (position >= w) {
        return 1.0;
    }
    return static_cast<float>(position) / (w);
}

std::size_t Horizontal_slider::percent_to_position(float percent) {
    std::size_t w{this->width()};
    if (w < 1) {
        return 0;
    }
    --w;
    return std::round(percent * w);
}

namespace slot {

sig::Slot<void(float)> set_percent(Horizontal_slider& s) {
    sig::Slot<void(float)> slot{
        [&s](float percent) { s.set_percent(percent); }};
    slot.track(s.destroyed);
    return slot;
}

sig::Slot<void()> set_percent(Horizontal_slider& s, float percent) {
    sig::Slot<void()> slot{[&s, percent] { s.set_percent(percent); }};
    slot.track(s.destroyed);
    return slot;
}

}  // namespace slot

}  // namespace cppurses
