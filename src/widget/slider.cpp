#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/widget/widgets/slider.hpp>

namespace cppurses {

Slider::Slider() {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
    this->background_tile = Glyph{"┊", foreground(Color::Light_gray)};
}

void Slider::set_percent(float percent) {
    if (percent < 0.0) {
        percent_progress_ = 0.0;
    } else if (percent > 1.0) {
        percent_progress_ = 1.0;
    } else {
        percent_progress_ = percent;
    }
    percent_changed(percent_progress_);
}

bool Slider::paint_event() {
    Painter p{this};
    std::size_t x{percent_to_position(percent_progress_)};
    p.put(indicator_, x, 0);
    return Widget::paint_event();
}

bool Slider::mouse_press_event(Mouse_button button,
                               std::size_t global_x,
                               std::size_t global_y,
                               std::size_t local_x,
                               std::size_t local_y,
                               std::uint8_t device_id) {
    percent_progress_ = position_to_percent(local_x);
    this->update();
    return Widget::mouse_press_event(button, global_x, global_y, local_x,
                                     local_y, device_id);
}

float Slider::position_to_percent(std::size_t position) {
    std::size_t w = this->width();
    if (w < 2) {
        return 0.0;
    }
    --w;
    if (position > w) {
        return 1.0;
    }
    return static_cast<float>(position) / (w - 1);
}

std::size_t Slider::percent_to_position(float percent) {
    std::size_t w{this->width()};
    if (w < 1) {
        return 0;
    }
    --w;
    return percent * w;
}

}  // namespace cppurses
