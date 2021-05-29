#include <termox/widget/widgets/slider.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/painter.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

template <template <typename> typename Layout_t, bool Inverted>
Slider<Layout_t, Inverted>::Slider(Range range, int initial)
    : range_{range}, value_{initial}
{
    if (range_.low > range_.high)
        throw std::invalid_argument{"Slider::Slider: Invalid Range."};
    if (value_ < range_.low || value_ > range_.high)
        throw std::out_of_range{"Slider::Slider: Invalid Initial Value."};
    range_changed.emit(range_);
    value_changed.emit(value_);
    *this | pipe::strong_focus();
}

template <template <typename> typename Layout_t, bool Inverted>
Slider<Layout_t, Inverted>::Slider(Parameters p) : Slider{p.range, p.initial}
{}

template <template <typename> typename Layout_t, bool Inverted>
void Slider<Layout_t, Inverted>::set_range(Range x)
{
    if (x.low > x.high)
        throw std::invalid_argument{"Slider::set_range: Invalid Range."};
    range_ = x;
    value_ = std::clamp(value_, range_.low, range_.high);
    this->update();
    value_changed.emit(value_);
    range_changed.emit(range_);
}

template <template <typename> typename Layout_t, bool Inverted>
auto Slider<Layout_t, Inverted>::range() const noexcept -> Range
{
    return range_;
}

template <template <typename> typename Layout_t, bool Inverted>
void Slider<Layout_t, Inverted>::set_value(int x)
{
    if (x < range_.low || x > range_.high)
        throw std::out_of_range{"Slider::set_value: Invalid Value."};
    value_ = x;
    this->update();
    value_changed.emit(value_);
}

template <template <typename> typename Layout_t, bool Inverted>
auto Slider<Layout_t, Inverted>::value() const noexcept -> int
{
    return value_;
}

template <template <typename> typename Layout_t, bool Inverted>
auto Slider<Layout_t, Inverted>::mouse_press_event(Mouse const& m) -> bool
{
    if (m.button == Mouse::Button::Left)
        this->set_value(this->value_from_point(m.at));
    return Widget::mouse_press_event(m);
}

template <template <typename> typename Layout_t, bool Inverted>
auto Slider<Layout_t, Inverted>::mouse_move_event(Mouse const& m) -> bool
{
    if (m.button == Mouse::Button::Left)
        this->set_value(this->value_from_point(m.at));
    return Widget::mouse_move_event(m);
}

template <template <typename> typename Layout_t, bool Inverted>
auto Slider<Layout_t, Inverted>::mouse_wheel_event(Mouse const& m) -> bool
{
    switch (m.button) {
        using ox::Mouse;
        case Mouse::Button::ScrollDown: {
            auto const amount = m.modifiers.shift ? extra() : 1;
            if constexpr (is_vertical && !Inverted)
                this->increment(amount);
            else
                this->decrement(amount);
            break;
        }
        case Mouse::Button::ScrollUp: {
            auto const amount = m.modifiers.shift ? extra() : 1;
            if constexpr (is_vertical && !Inverted)
                this->decrement(amount);
            else
                this->increment(amount);
            break;
        }
        default: break;
    }
    return Widget::mouse_wheel_event(m);
}

template <template <typename> typename Layout_t, bool Inverted>
auto Slider<Layout_t, Inverted>::key_press_event(Key k) -> bool
{
    switch (k) {
        using ox::Key;
        using ox::Mod;
        case Key::Arrow_right: {
            if constexpr (!is_vertical && Inverted)
                this->decrement();
            else
                this->increment();
            break;
        }
        case Key::Arrow_left: {
            if constexpr (!is_vertical && Inverted)
                this->increment();
            else
                this->decrement();
            break;
        }
        case Key::Arrow_up: {
            if constexpr (is_vertical && !Inverted)
                this->decrement();
            else
                this->increment();
            break;
        }
        case Key::Arrow_down: {
            if constexpr (is_vertical && !Inverted)
                this->increment();
            else
                this->decrement();
            break;
        }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
        case Key::Arrow_right | Mod::Shift: {
            if constexpr (!is_vertical && Inverted)
                this->decrement(extra());
            else
                this->increment(extra());
            break;
        }
        case Key::Arrow_left | Mod::Shift: {
            if constexpr (!is_vertical && Inverted)
                this->increment(extra());
            else
                this->decrement(extra());
            break;
        }
        case Key::Arrow_up | Mod::Shift: {
            if constexpr (is_vertical && !Inverted)
                this->decrement(extra());
            else
                this->increment(extra());
            break;
        }
        case Key::Arrow_down | Mod::Shift: {
            if constexpr (is_vertical && !Inverted)
                this->increment(extra());
            else
                this->decrement(extra());
            break;
        }
#pragma GCC diagnostic pop
        default: break;
    }
    return Widget::key_press_event(k);
}

template <template <typename> typename Layout_t, bool Inverted>
auto Slider<Layout_t, Inverted>::paint_event(Painter& p) -> bool
{
    auto const ratio =
        (double)(value_ - range_.low + 1) / (double)length(range_);
    auto const w_length = [this] {
        if constexpr (is_vertical)
            return this->area().height;
        else
            return this->area().width;
    }();

    // Beginning index to draw the slider bar.
    auto const begin = [&] {
        if constexpr (Inverted) {
            if constexpr (is_vertical)
                return this->area().height - 1;
            else
                return this->area().width - 1;
        }
        else
            return 0;
    }();

    // Ending index to draw the slider bar.
    auto const end = [&] {
        auto const pos = (int)std::ceil(ratio * w_length) - 1;
        if constexpr (Inverted)
            return w_length - pos - 1;
        else
            return pos;
    }();

    // Mid section of slider display.
    auto const top_left = [&] {
        if constexpr (Inverted)
            return make_point(end, 0);
        else
            return make_point(begin, 0);
    }();
    auto const breadth = is_vertical ? this->area().width : this->area().height;
    auto const area    = [&] {
        if constexpr (Inverted)
            return make_area(w_length - end, breadth);
        else
            return make_area(end, breadth);
    }();
    p.fill(U' ' | Trait::Inverse, top_left, area);

    // End line of slider display.
    auto const end_block = [&] {
        auto constexpr blocks = std::array{U'░', U'▒', U'▓', U'█'};
        std::size_t const index =
            (int)(std::ceil(ratio * w_length * blocks.size()) - 1) %
            blocks.size();
        return blocks[index];
    }();
    for (auto across = 0; across != breadth; ++across)
        p.put(end_block, make_point(end, across));

    return Widget::paint_event(p);
}

template <template <typename> typename Layout_t, bool Inverted>
void Slider<Layout_t, Inverted>::increment(int amount)
{
    this->set_value(std::clamp(value_ + amount, range_.low, range_.high));
}

template <template <typename> typename Layout_t, bool Inverted>
void Slider<Layout_t, Inverted>::decrement(int amount)
{
    this->set_value(std::clamp(value_ - amount, range_.low, range_.high));
}

template <template <typename> typename Layout_t, bool Inverted>
auto Slider<Layout_t, Inverted>::value_from_point(Point p) const -> int
{
    auto const w_length =
        is_vertical ? this->area().height : this->area().width;
    auto const pos = [&] {
        auto const x = [&] {
            if constexpr (is_vertical)
                return p.y;
            else
                return p.x;
        }();
        if constexpr (Inverted)
            return w_length - x - 1;
        else
            return x;
    }();
    auto const ratio  = (double)pos / (double)w_length;
    auto const offset = (int)(ratio * length(range_));
    return range_.low + offset;
}

template <template <typename> typename Layout_t, bool Inverted>
auto Slider<Layout_t, Inverted>::extra() const -> int
{
    auto constexpr extra_percent = 0.07;
    return std::ceil(extra_percent * length(range_));
}

template <template <typename> typename Layout_t, bool Inverted>
auto Slider<Layout_t, Inverted>::length(Range range) -> int
{
    return range.high - range.low + 1;
}

template <template <typename> typename Layout_t, bool Inverted>
auto Slider<Layout_t, Inverted>::make_point(int length, int breadth) -> Point
{
    if constexpr (is_vertical)
        return {breadth, length};
    else
        return {length, breadth};
}

template <template <typename> typename Layout_t, bool Inverted>
auto Slider<Layout_t, Inverted>::make_area(int length_dim, int breadth_dim)
    -> Area
{
    if constexpr (is_vertical)
        return {breadth_dim, length_dim};
    else
        return {length_dim, breadth_dim};
}

template class Slider<layout::Horizontal, true>;
template class Slider<layout::Horizontal, false>;
template class Slider<layout::Vertical, true>;
template class Slider<layout::Vertical, false>;

template <template <typename> typename Layout_t, bool Inverted>
auto slider(typename Slider<Layout_t, Inverted>::Range range, int initial)
    -> std::unique_ptr<Slider<Layout_t, Inverted>>
{
    return std::make_unique<Slider<Layout_t, Inverted>>(range, initial);
}

template auto slider(Slider<layout::Horizontal, true>::Range range, int initial)
    -> std::unique_ptr<Slider<layout::Horizontal, true>>;
template auto slider(Slider<layout::Horizontal, false>::Range range,
                     int initial)
    -> std::unique_ptr<Slider<layout::Horizontal, false>>;
template auto slider(Slider<layout::Vertical, true>::Range range, int initial)
    -> std::unique_ptr<Slider<layout::Vertical, true>>;
template auto slider(Slider<layout::Vertical, false>::Range range, int initial)
    -> std::unique_ptr<Slider<layout::Vertical, false>>;

template <template <typename> typename Layout_t, bool Inverted>
auto slider(typename Slider<Layout_t, Inverted>::Parameters parameters)
    -> std::unique_ptr<Slider<Layout_t, Inverted>>
{
    return std::make_unique<Slider<Layout_t, Inverted>>(std::move(parameters));
}

template auto slider(
    typename Slider<layout::Horizontal, true>::Parameters parameters)
    -> std::unique_ptr<Slider<layout::Horizontal, true>>;
template auto slider(
    typename Slider<layout::Horizontal, false>::Parameters parameters)
    -> std::unique_ptr<Slider<layout::Horizontal, false>>;
template auto slider(
    typename Slider<layout::Vertical, true>::Parameters parameters)
    -> std::unique_ptr<Slider<layout::Vertical, true>>;
template auto slider(
    typename Slider<layout::Vertical, false>::Parameters parameters)
    -> std::unique_ptr<Slider<layout::Vertical, false>>;

auto slider_top(Slider_top::Range range, int initial)
    -> std::unique_ptr<Slider_top>
{
    return std::make_unique<Slider_top>(range, initial);
}

auto slider_top(Slider_top::Parameters parameters)
    -> std::unique_ptr<Slider_top>
{
    return std::make_unique<Slider_top>(std::move(parameters));
}

auto slider_bottom(Slider_bottom::Range range, int initial)
    -> std::unique_ptr<Slider_bottom>
{
    return std::make_unique<Slider_bottom>(range, initial);
}

auto slider_bottom(Slider_bottom::Parameters parameters)
    -> std::unique_ptr<Slider_bottom>
{
    return std::make_unique<Slider_bottom>(std::move(parameters));
}

auto slider_left(Slider_left::Range range, int initial)
    -> std::unique_ptr<Slider_left>
{
    return std::make_unique<Slider_left>(range, initial);
}

auto slider_left(Slider_left::Parameters parameters)
    -> std::unique_ptr<Slider_left>
{
    return std::make_unique<Slider_left>(std::move(parameters));
}

auto slider_right(Slider_right::Range range, int initial)
    -> std::unique_ptr<Slider_right>
{
    return std::make_unique<Slider_right>(range, initial);
}

auto slider_right(Slider_right::Parameters parameters)
    -> std::unique_ptr<Slider_right>
{
    return std::make_unique<Slider_right>(std::move(parameters));
}

}  // namespace ox
