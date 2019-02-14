#include <cppurses/widget/widgets/vertical_slider.hpp>

#include <cmath>
#include <cstddef>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/widgets/detail/nearly_equal.hpp>

namespace cppurses {

Vertical_slider::Vertical_slider(Value_t minimum, Value_t maximum)
    : logic_{minimum, maximum} {
    this->focus_policy = Focus_policy::Strong;
}

void Vertical_slider::set_value(Value_t value) {
    logic_.set_value(value);
    this->update();
}

void Vertical_slider::set_ratio(Ratio_t ratio) {
    logic_.set_ratio(ratio);
    this->update();
}

void Vertical_slider::increment(Value_t amount) {
    inverted_ ? logic_.decrement() : logic_.increment();
    this->update();
}

void Vertical_slider::decrement(Value_t amount) {
    inverted_ ? logic_.increment() : logic_.decrement();
    this->update();
}

void Vertical_slider::set_minimum(Value_t min) {
    logic_.set_minimum(min);
    this->update();
}

void Vertical_slider::set_maximum(Value_t max) {
    logic_.set_maximum(max);
    this->update();
}

void Vertical_slider::set_upper(const Glyph_string& symbols, bool repeats) {
    upper_ = symbols;
    upper_repeats_ = repeats;
    this->update();
}

void Vertical_slider::set_indicator(const Glyph_string& symbols) {
    indicator_ = symbols;
    this->update();
}

void Vertical_slider::set_lower(const Glyph_string& symbols, bool repeats) {
    lower_ = symbols;
    lower_repeats_ = repeats;
    this->update();
}

bool Vertical_slider::paint_event() {
    Painter p{*this};
    const auto indicator_y = indicator_position();
    const auto width = this->width();
    // Upper
    const auto& upper_gs = inverted_ ? lower_ : upper_;
    const auto up_repeats = inverted_ ? lower_repeats_ : upper_repeats_;
    const auto upper_limit = indicator_y - 1;
    for (auto y = indicator_y - 1; y < indicator_y; --y) {
        const auto upper_index = (upper_limit - y) % upper_gs.size();
        for (auto x = 0; x < width; ++x) {
            p.put(upper_gs[upper_index], x, y);
        }
        if (!up_repeats && (indicator_y - y) == upper_gs.size()) {
            break;
        }
    }
    // Middle
    const std::size_t index = this->indicator_index();
    for (auto x = 0; x < width; ++x) {
        p.put(indicator_[index], x, indicator_y);
    }
    // Lower
    const auto& lower_gs = inverted_ ? upper_ : lower_;
    const auto low_repeats = inverted_ ? upper_repeats_ : lower_repeats_;
    const auto height = this->height();
    const auto lower_offset = indicator_y + 1;
    for (auto y = indicator_y + 1; y < height; ++y) {
        const auto lower_index = (y - lower_offset) % lower_gs.size();
        for (auto x = 0; x < width; ++x) {
            p.put(lower_gs[lower_index], x, y);
        }
        if (!low_repeats && (y - indicator_y) == lower_gs.size()) {
            break;
        }
    }
    return Widget::paint_event();
}

bool Vertical_slider::mouse_press_event(const Mouse_data& mouse) {
    switch (mouse.button) {
        case Mouse_button::Left:
            logic_.set_ratio(this->ratio_at(mouse.local.y));
            this->update();
            break;
        case Mouse_button::ScrollUp:
            this->increment();
            break;
        case Mouse_button::ScrollDown:
            this->decrement();
            break;
        default:
            break;
    }
    return Widget::mouse_press_event(mouse);
}

bool Vertical_slider::key_press_event(const Keyboard_data& keyboard) {
    switch (keyboard.key) {
        case Key::Arrow_up:
            this->increment();
            break;
        case Key::Arrow_down:
            this->decrement();
            break;
        default:
            break;
    }
    return Widget::key_press_event(keyboard);
}

Vertical_slider::Ratio_t Vertical_slider::ratio_at(std::size_t position) const {
    if (this->height() == 1) {
        return 1.0;
    }
    const auto screen_max = this->height() - 1;
    position = inverted_ ? position : screen_max - position;
    return static_cast<Ratio_t>(position) / screen_max;
}

Vertical_slider::Ratio_t Vertical_slider::indicator_absolute_position() const {
    const auto screen_max = this->height() - 1;
    return detail::ceil_if_nearly_whole(logic_.ratio() * screen_max);
}

std::size_t Vertical_slider::indicator_position() const {
    const auto screen_max = this->height() - 1;
    const auto abs_pos = std::floor(this->indicator_absolute_position());
    return inverted_ ? abs_pos : screen_max - abs_pos;
}

std::size_t Vertical_slider::indicator_index() const {
    const std::size_t indicator_n =
        std::floor(this->indicator_absolute_position() * indicator_.size());
    return indicator_n % indicator_.size();
}

}  // namespace cppurses
