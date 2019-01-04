#include <cppurses/widget/widgets/vertical_slider.hpp>

#include <cmath>
#include <cstddef>
#include <fstream>  //temp

#include <cppurses/painter/painter.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/size_policy.hpp>

namespace {
using namespace cppurses;

// Finds the position along a distance that percent corresponds to.
std::size_t position_at(float percent, std::size_t distance) {
    std::size_t result = distance - std::round(percent * distance);
    if (result == distance) {
        --result;
    }
    return result;
}

}  // namespace

namespace cppurses {

Vertical_slider::Vertical_slider(std::size_t limit) : logic_{limit} {
    this->focus_policy = Focus_policy::Strong;
}

bool Vertical_slider::paint_event() {
    Painter p{*this};
    auto middle_position = static_cast<std::size_t>(
        this->height() - (logic_.ratio() * this->height()));
    if (middle_position == this->height()) {
        --middle_position;
    }
    for (auto x = 0; x < this->width(); ++x) {
        p.put(middle_, x, middle_position);
    }

    // auto middle_pos = position_at(position_, this->height());
    // // Upper
    // for (auto y = 0; y < middle_pos; ++y) {
    //     for (auto x = 0; x < this->width(); ++x) {
    //         p.put(upper_, x, y);
    //     }
    // }

    // // Middle
    // auto mid_glyph = middle_[this->middle_index()];
    // if (position_ == 1.0) {
    //     mid_glyph = lower_;
    // }
    // for (auto x = 0; x < this->width(); ++x) {
    //     p.put(mid_glyph, x, middle_pos);
    // }

    // // Lower
    // for (auto y = middle_pos + 1; y < this->height(); ++y) {
    //     for (auto x = 0; x < this->width(); ++x) {
    //         p.put(lower_, x, y);
    //     }
    // }
    return Widget::paint_event();
}

bool Vertical_slider::mouse_press_event(const Mouse_data& mouse) {
    if (this->height() == 0) {
        return Widget::mouse_press_event(mouse);
    }
    if (mouse.button == Mouse_button::Left) {
        const auto new_value = static_cast<std::size_t>(
            (static_cast<double>((this->height() - 1) - mouse.local.y) /
             (this->height() - 1)) *
            logic_.limit());
        logic_.set_value(new_value);
        std::ofstream log{"log", std::ios::app};
        log << "value: " << logic_.value() << std::endl;
    } else if (mouse.button == Mouse_button::ScrollUp) {
        logic_.increment();
    } else if (mouse.button == Mouse_button::ScrollDown) {
        logic_.decrement();
    }
    this->update();
    return Widget::mouse_press_event(mouse);
}

bool Vertical_slider::key_press_event(const Keyboard_data& keyboard) {
    if (keyboard.key == Key::Arrow_up) {
        logic_.increment();
    } else if (keyboard.key == Key::Arrow_down) {
        logic_.decrement();
    }
    this->update();
    return Widget::key_press_event(keyboard);
}

// std::size_t Vertical_slider::middle_index() const {
//     const auto result =
//         static_cast<std::size_t>(this->height() * middle_.size() * position_)
//         % middle_.size();
//     return result;
// }

}  // namespace cppurses
