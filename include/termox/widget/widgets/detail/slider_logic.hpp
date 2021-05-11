#ifndef TERMOX_WIDGET_WIDGETS_DETAIL_SLIDER_LOGIC_HPP
#define TERMOX_WIDGET_WIDGETS_DETAIL_SLIDER_LOGIC_HPP
#include <signals_light/signal.hpp>

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
    Slider_logic(Value_t minimum, Value_t maximum);

   public:
    /// Set the minimum value the slider can take on.
    /** Modifies maximum to be min + 1 if max <= min. */
    void set_minimum(Value_t min);

    /// Set the maximum value the slider can take on.
    /** Modifies minimum to be max - 1 if min >= max. */
    void set_maximum(Value_t max);

    /// Return the smallest possible value for the slider.
    [[nodiscard]] auto minimum() const -> Value_t;

    /// Return the largest possible value for the slider.
    [[nodiscard]] auto maximum() const -> Value_t;

    /// Increment the current value by \p amount.
    void increment(Value_t amount = 1);

    /// Decrement the current value by \p amount.
    void decrement(Value_t amount = 1);

    /// Directly set the ratio as a value from [0.0, 1.0].
    void set_ratio(Ratio_t ratio);

    /// Return the position of the current value in the slider as a ratio.
    [[nodiscard]] auto ratio() const -> double;

    /// Set the current value of the slider and emit signal.
    void set_value(Value_t value);

    /// Return the current value.
    [[nodiscard]] auto value() const -> Value_t;

    /// Return the distance between the maximum and minimum.
    [[nodiscard]] auto length() const -> Value_t;

   private:
    Value_t minimum_ = 0;
    Value_t maximum_ = 0;
    Ratio_t ratio_   = 0.;
};

}  // namespace ox::detail
#endif  // TERMOX_WIDGET_WIDGETS_DETAIL_SLIDER_LOGIC_HPP
