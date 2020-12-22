#include <termox/widget/widgets/vertical_slider.hpp>

#include <cmath>
#include <cstddef>

#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/widgets/detail/nearly_equal.hpp>

namespace ox {

auto Vertical_slider::paint_event() -> bool
{
    Painter p{*this};
    auto const indicator_y = indicator_position();
    auto const width       = this->width();

    // Upper
    auto const& upper_gs   = inverted_ ? lower_ : upper_;
    auto const up_repeats  = inverted_ ? lower_repeats_ : upper_repeats_;
    auto const upper_limit = indicator_y - 1;
    for (auto y = indicator_y - 1; y < indicator_y; --y) {
        auto const upper_index = (upper_limit - y) % upper_gs.size();
        for (auto x = 0uL; x < width; ++x) {
            p.put(upper_gs[upper_index], x, y);
        }
        if (not up_repeats and (indicator_y - y) == upper_gs.size())
            break;
    }

    // Middle
    auto const index = this->indicator_index();
    for (auto x = 0uL; x < width; ++x) {
        p.put(indicator_[index], x, indicator_y);
    }

    // Lower
    auto const& lower_gs    = inverted_ ? upper_ : lower_;
    auto const low_repeats  = inverted_ ? upper_repeats_ : lower_repeats_;
    auto const height       = this->height();
    auto const lower_offset = indicator_y + 1;
    for (auto y = indicator_y + 1; y < height; ++y) {
        auto const lower_index = (y - lower_offset) % lower_gs.size();
        for (auto x = 0uL; x < width; ++x) {
            p.put(lower_gs[lower_index], x, y);
        }
        if (not low_repeats and (y - indicator_y) == lower_gs.size())
            break;
    }
    return Widget::paint_event();
}

auto Vertical_slider::mouse_press_event(const Mouse& m) -> bool
{
    if (m.button == Mouse::Button::Left) {
        logic_.set_ratio(this->ratio_at(m.local.y));
        this->update();
    }
    return Widget::mouse_press_event(m);
}

auto Vertical_slider::mouse_wheel_event(Mouse const& m) -> bool
{
    switch (m.button) {
        case Mouse::Button::ScrollUp: this->increment(); break;
        case Mouse::Button::ScrollDown: this->decrement(); break;
        default: break;
    }
    return Widget::mouse_wheel_event(m);
}

auto Vertical_slider::ratio_at(std::size_t position) const -> Ratio_t
{
    if (this->height() == 1)
        return 1.0;
    auto const screen_max = this->height() - 1;
    position              = inverted_ ? position : screen_max - position;
    return static_cast<Ratio_t>(position) / screen_max;
}

auto Vertical_slider::indicator_absolute_position() const -> Ratio_t
{
    auto const screen_max = this->height() - 1;
    return detail::ceil_if_nearly_whole(logic_.ratio() * screen_max);
}

auto Vertical_slider::indicator_position() const -> std::size_t
{
    auto const screen_max = this->height() - 1;
    auto const abs_pos    = std::floor(this->indicator_absolute_position());
    return inverted_ ? abs_pos : screen_max - abs_pos;
}

auto Vertical_slider::indicator_index() const -> std::size_t
{
    auto const indicator_n = static_cast<std::size_t>(
        std::floor(this->indicator_absolute_position() * indicator_.size()));
    return indicator_n % indicator_.size();
}

}  // namespace ox
