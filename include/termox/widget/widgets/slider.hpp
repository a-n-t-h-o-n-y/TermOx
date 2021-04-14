#ifndef TERMOX_WIDGET_WIDGETS_SLIDER_HPP
#define TERMOX_WIDGET_WIDGETS_SLIDER_HPP
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <memory>
#include <stdexcept>

#include <signals_light/signal.hpp>

#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// Fixed range sliding display fixed at one end of the Widget.
/** Range can contain negative values. */
template <template <typename> typename Layout_t, bool Inverted>
class Slider : public Widget {
   public:
    /// [low, high] inclusive range of values.
    struct Range {
        int low;
        int high;
    };

    struct Parameters {
        Range range;
        int initial;
    };

   public:
    /// Emitted when the [low, high] range is modified.
    sl::Signal<void(Range)> range_changed;

    /// Emitted on user input and passes along the new current value set.
    sl::Signal<void(int)> value_changed;

   public:
    /// Create new Slider.
    /** Throws std::invalid_argument if \p range's low is greater than its high.
     *  Throws std::out_of_range if \p initial is outside of \p range. */
    Slider(Range range, int initial) : range_{range}, value_{initial}
    {
        if (range_.low > range_.high)
            throw std::invalid_argument{"Slider::Slider: Invalid Range."};
        if (value_ < range_.low || value_ > range_.high)
            throw std::out_of_range{"Slider::Slider: Invalid Initial Value."};
        range_changed.emit(range_);
        value_changed.emit(value_);
        *this | pipe::strong_focus();
    }

    /// Create new Slider.
    /** Throws std::invalid_argument if \p range's low is greater than its high.
     *  Throws std::out_of_range if \p initial is outside of \p range. */
    explicit Slider(Parameters p) : Slider{p.range, p.initial} {}

   public:
    /// Set a new Range for this Slider.
    /** Throws std::invalid_argument if \p range's low is greater than its high.
     *  The current value is clamped to the new range, if now outside of it. */
    void set_range(Range x)
    {
        if (x.low > x.high)
            throw std::invalid_argument{"Slider::set_range: Invalid Range."};
        range_ = x;
        value_ = std::clamp(value_, range_.low, range_.high);
        this->update();
        value_changed.emit(value_);
        range_changed.emit(range_);
    }

    /// Return the currently set Range. Emits range_changed Signal.
    [[nodiscard]] auto range() const noexcept -> Range { return range_; }

    /// Set the current value of the Slider. Emits value_changed Signal.
    /** Throws std::out_of_range if outside of the currently set Range. */
    void set_value(int x)
    {
        if (x < range_.low || x > range_.high)
            throw std::out_of_range{"Slider::set_value: Invalid Value."};
        value_ = x;
        this->update();
        value_changed.emit(value_);
    }

    /// Return the currently set value.
    [[nodiscard]] auto value() const noexcept -> int { return value_; }

   protected:
    auto mouse_press_event(Mouse const& m) -> bool override
    {
        if (m.button == Mouse::Button::Left)
            this->set_value(this->value_from_point(m.at));
        return Widget::mouse_press_event(m);
    }

    auto mouse_move_event(Mouse const& m) -> bool override
    {
        if (m.button == Mouse::Button::Left)
            this->set_value(this->value_from_point(m.at));
        return Widget::mouse_move_event(m);
    }

    auto mouse_wheel_event(Mouse const& m) -> bool override
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

    auto key_press_event(Key k) -> bool override
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

    auto paint_event(Painter& p) -> bool override
    {
        auto const ratio =
            (double)(value_ - range_.low + 1) / (double)length(range_);
        auto const w_length = [this] {
            if constexpr (is_vertical)
                return this->height();
            else
                return this->width();
        }();

        // Beginning index to draw the slider bar.
        auto const begin = [&] {
            if constexpr (Inverted) {
                if constexpr (is_vertical)
                    return this->height() - 1;
                else
                    return this->width() - 1;
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
        auto const breadth = is_vertical ? this->width() : this->height();
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

   private:
    void increment(int amount = 1)
    {
        this->set_value(std::clamp(value_ + amount, range_.low, range_.high));
    }

    void decrement(int amount = 1)
    {
        this->set_value(std::clamp(value_ - amount, range_.low, range_.high));
    }

    /// Return the value that should be at the given display Point.
    [[nodiscard]] auto value_from_point(Point p) const -> int
    {
        auto const w_length = is_vertical ? this->height() : this->width();
        auto const pos      = [&] {
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

   private:
    /// Return some percentage of the current range for long increment.
    [[nodiscard]] auto extra() const -> int
    {
        auto constexpr extra_percent = 0.07;
        return std::ceil(extra_percent * length(range_));
    }

    /// Return the number of whole values in the currently set Range.
    [[nodiscard]] static auto length(Range range) -> int
    {
        return range.high - range.low + 1;
    }

    [[nodiscard]] static auto make_point(int length, int breadth) -> Point
    {
        if constexpr (is_vertical)
            return {breadth, length};
        else
            return {length, breadth};
    }

    [[nodiscard]] static auto make_area(int length_dim, int breadth_dim) -> Area
    {
        if constexpr (is_vertical)
            return {breadth_dim, length_dim};
        else
            return {length_dim, breadth_dim};
    }

   private:
    static auto constexpr is_vertical = layout::is_vertical_v<Layout_t<Widget>>;

    Range range_;
    int value_;
};

/// Helper function to create a Slider instance.
template <template <typename> typename Layout_t, bool Inverted>
[[nodiscard]] auto slider(typename Slider<Layout_t, Inverted>::Range range,
                          int initial)
    -> std::unique_ptr<Slider<Layout_t, Inverted>>
{
    return std::make_unique<Slider<Layout_t, Inverted>>(range, initial);
}

/// Helper function to create a Slider instance.
template <template <typename> typename Layout_t, bool Inverted>
[[nodiscard]] auto slider(
    typename Slider<Layout_t, Inverted>::Parameters parameters)
    -> std::unique_ptr<Slider<Layout_t, Inverted>>
{
    return std::make_unique<Slider<Layout_t, Inverted>>(std::move(parameters));
}

using Slider_top = Slider<layout::Vertical, false>;

/// Helper function to create a Slider_top instance.
[[nodiscard]] inline auto slider_top(Slider_top::Range range, int initial)
    -> std::unique_ptr<Slider_top>
{
    return std::make_unique<Slider_top>(range, initial);
}

/// Helper function to create a Slider_top instance.
[[nodiscard]] inline auto slider_top(Slider_top::Parameters parameters)
    -> std::unique_ptr<Slider_top>
{
    return std::make_unique<Slider_top>(std::move(parameters));
}

using Slider_bottom = Slider<layout::Vertical, true>;

/// Helper function to create a Slider_bottom instance.
[[nodiscard]] inline auto slider_bottom(Slider_bottom::Range range, int initial)
    -> std::unique_ptr<Slider_bottom>
{
    return std::make_unique<Slider_bottom>(range, initial);
}

/// Helper function to create a Slider_bottom instance.
[[nodiscard]] inline auto slider_bottom(Slider_bottom::Parameters parameters)
    -> std::unique_ptr<Slider_bottom>
{
    return std::make_unique<Slider_bottom>(std::move(parameters));
}

using Slider_left = Slider<layout::Horizontal, false>;

/// Helper function to create a Slider_left instance.
[[nodiscard]] inline auto slider_left(Slider_left::Range range, int initial)
    -> std::unique_ptr<Slider_left>
{
    return std::make_unique<Slider_left>(range, initial);
}

/// Helper function to create a Slider_left instance.
[[nodiscard]] inline auto slider_left(Slider_left::Parameters parameters)
    -> std::unique_ptr<Slider_left>
{
    return std::make_unique<Slider_left>(std::move(parameters));
}

using Slider_right = Slider<layout::Horizontal, true>;

/// Helper function to create a Slider_right instance.
[[nodiscard]] inline auto slider_right(Slider_right::Range range, int initial)
    -> std::unique_ptr<Slider_right>
{
    return std::make_unique<Slider_right>(range, initial);
}

/// Helper function to create a Slider_right instance.
[[nodiscard]] inline auto slider_right(Slider_right::Parameters parameters)
    -> std::unique_ptr<Slider_right>
{
    return std::make_unique<Slider_right>(std::move(parameters));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_SLIDER_HPP
