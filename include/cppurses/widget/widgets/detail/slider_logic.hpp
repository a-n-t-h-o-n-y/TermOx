#ifndef CPPURSES_WIDGET_WIDGETS_DETAIL_SLIDER_LOGIC_HPP
#define CPPURSES_WIDGET_WIDGETS_DETAIL_SLIDER_LOGIC_HPP
#include <signals/signal.hpp>

namespace cppurses {
namespace detail {

/// Holds the ratio of a slider like Widget and calculates current value.
class Slider_logic {
   public:
    using Value_t = int;
    using Ratio_t = double;

    /// Initialize with range of acceptable values and minimum current value.
    Slider_logic(Value_t minimum, Value_t maximum);

    /// Set the minimum value the slider can take on.
    /** Modifies maximum to be min + 1 if max <= min. */
    void set_minimum(Value_t min);

    /// Set the maximum value the slider can take on.
    /** Modifies minimum to be max - 1 if min >= max. */
    void set_maximum(Value_t max);

    /// Return the smallest possible value for the slider.
    Value_t minimum() const { return minimum_; }

    /// Return the largest possible value for the slider.
    Value_t maximum() const { return maximum_; }

    /// Increment the current value by \p amount.
    void increment(Value_t amount = 1) {
        this->set_value(this->value() + amount);
    }

    /// Decrement the current value by \p amount.
    void decrement(Value_t amount = 1) {
        this->set_value(this->value() - amount);
    }

    /// Directly set the ratio as a value from [0.0, 1.0].
    void set_ratio(Ratio_t ratio);

    /// Return the position of the current value in the slider as a ratio.
    double ratio() const { return ratio_; };

    /// Set the current value of the slider and emit signal.
    void set_value(Value_t value);

    /// Return the current value.
    Value_t value() const;

    /// Return the distance between the maximum and minimum.
    Value_t length() const { return maximum_ - minimum_; }

    /// Signal emitted every time the value is changed.
    sig::Signal<void(Value_t)> value_changed;

    /// Signal emitted every time the ratio is changed.
    sig::Signal<void(Ratio_t)> ratio_changed;

   private:
    Value_t minimum_{0};
    Value_t maximum_{0};
    Ratio_t ratio_{0.0};
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_DETAIL_SLIDER_LOGIC_HPP
