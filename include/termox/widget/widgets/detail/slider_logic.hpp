#ifndef TERMOX_WIDGET_WIDGETS_DETAIL_SLIDER_LOGIC_HPP
#define TERMOX_WIDGET_WIDGETS_DETAIL_SLIDER_LOGIC_HPP
#include <cmath>
#include <limits>
#include <stdexcept>

#include <signals_light/signal.hpp>

#include <termox/widget/widgets/detail/nearly_equal.hpp>

namespace ox::detail {

/// Holds the ratio of a slider like Widget and calculates current value.
class Slider_logic {
   public:
    using Value_t = int;
    using Ratio_t = double;

    /// Signal emitted every time the value is changed.
    sl::Signal<void(Value_t)> value_changed;

    /// Signal emitted every time the ratio is changed.
    sl::Signal<void(Ratio_t)> ratio_changed;

   public:
    /// Initialize with range of acceptable values and minimum current value.
    Slider_logic(Value_t minimum, Value_t maximum)
    {
        this->set_minimum(minimum);
        this->set_maximum(maximum);
        this->set_value(minimum_);
    }

    /// Set the minimum value the slider can take on.
    /** Modifies maximum to be min + 1 if max <= min. */
    void set_minimum(Value_t min)
    {
        if (std::numeric_limits<Value_t>::max() == min)
            throw std::overflow_error{"Slider_logic::set_minimum."};
        if (maximum_ <= min)
            maximum_ = min + 1;
        minimum_ = min;
    }

    /// Set the maximum value the slider can take on.
    /** Modifies minimum to be max - 1 if min >= max. */
    void set_maximum(Value_t max)
    {
        if (std::numeric_limits<Value_t>::min() == max)
            throw std::underflow_error{"Slider_logic::set_maximum."};
        if (minimum_ >= max)
            minimum_ = max - 1;
        maximum_ = max;
    }

    /// Return the smallest possible value for the slider.
    auto minimum() const -> Value_t { return minimum_; }

    /// Return the largest possible value for the slider.
    auto maximum() const -> Value_t { return maximum_; }

    /// Increment the current value by \p amount.
    void increment(Value_t amount = 1)
    {
        this->set_value(this->value() + amount);
    }

    /// Decrement the current value by \p amount.
    void decrement(Value_t amount = 1)
    {
        this->set_value(this->value() - amount);
    }

    /// Directly set the ratio as a value from [0.0, 1.0].
    void set_ratio(Ratio_t ratio)
    {
        if (ratio < 0.0)
            ratio = 0.0;
        else if (ratio > 1.0)
            ratio = 1.0;
        auto const emit_signal = (ratio_ != ratio);
        if (emit_signal) {
            ratio_ = ratio;
            ratio_changed(ratio);
            value_changed(this->value());
        }
    }

    /// Return the position of the current value in the slider as a ratio.
    auto ratio() const -> double { return ratio_; };

    /// Set the current value of the slider and emit signal.
    void set_value(Value_t value)
    {
        if (value > maximum_)
            value = maximum_;
        else if (value < minimum_)
            value = minimum_;
        auto const ratio =
            static_cast<double>(value - minimum_) / this->length();
        this->set_ratio(ratio);
    }

    /// Return the current value.
    auto value() const -> Value_t
    {
        return minimum_ + std::floor(detail::ceil_if_nearly_whole(
                              ratio_ * this->length()));
    }

    /// Return the distance between the maximum and minimum.
    auto length() const -> Value_t { return maximum_ - minimum_; }

   private:
    Value_t minimum_ = 0;
    Value_t maximum_ = 0;
    Ratio_t ratio_   = 0.;
};

}  // namespace ox::detail
#endif  // TERMOX_WIDGET_WIDGETS_DETAIL_SLIDER_LOGIC_HPP
