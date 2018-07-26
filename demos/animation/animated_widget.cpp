#include "animated_widget.hpp"

#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/focus_policy.hpp>

namespace demos {
namespace animation {

Animated_bit::Animated_bit(int rate) {
    this->focus_policy = cppurses::Focus_policy::Strong;
    this->enable_animation(rate);
}

bool Animated_bit::animation_event() {
    int next_x = coords_.x + (1 * x_direction);
    if (next_x >= this->width() || next_x < 0) {
        x_direction *= -1;
        next_x = coords_.x + (1 * x_direction);
    }
    int next_y = coords_.y + (1 * y_direction);
    if (next_y >= this->height() || next_y < 0) {
        y_direction *= -1;
        next_y = coords_.y + (1 * y_direction);
    }
    coords_.x = next_x;
    coords_.y = next_y;
    return Widget::animation_event();
}

bool Animated_bit::paint_event() {
    Painter p{this};
    p.put(glyph_, coords_);
    return Widget::paint_event();
}

bool Animated_bit::mouse_press_event(cppurses::Mouse_button button,
                                     cppurses::Point global,
                                     cppurses::Point local,
                                     std::uint8_t device_id) {
    coords_ = local;
    this->update();
    return Widget::mouse_press_event(button, global, local, device_id);
}

}  // namespace animation
}  // namespace demos
