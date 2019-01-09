#include <cppurses/widget/widgets/detail/slider_logic.hpp>

#include <cmath>
#include <limits>
#include <stdexcept>

#include <signals/signal.hpp>

#include <cppurses/widget/widgets/detail/nearly_equal.hpp>

namespace cppurses {
namespace detail {

Slider_logic::Slider_logic(Value_t minimum, Value_t maximum) {
    this->set_minimum(minimum);
    this->set_maximum(maximum);
    this->set_value(minimum_);
}

void Slider_logic::set_minimum(Value_t min) {
    if (std::numeric_limits<Value_t>::max() == min) {
        throw std::overflow_error{"Slider_logic::set_minimum."};
    }
    if (maximum_ <= min) {
        maximum_ = min + 1;
    }
    minimum_ = min;
}

void Slider_logic::set_maximum(Value_t max) {
    if (std::numeric_limits<Value_t>::min() == max) {
        throw std::underflow_error{"Slider_logic::set_maximum."};
    }
    if (minimum_ >= max) {
        minimum_ = max - 1;
    }
    maximum_ = max;
}

void Slider_logic::set_value(Value_t value) {
    if (value > maximum_) {
        value = maximum_;
    } else if (value < minimum_) {
        value = minimum_;
    }
    double ratio = static_cast<double>(value - minimum_) / this->length();
    this->set_ratio(ratio);
}

void Slider_logic::set_ratio(Ratio_t ratio) {
    if (ratio < 0.0) {
        ratio = 0.0;
    } else if (ratio > 1.0) {
        ratio = 1.0;
    }
    const bool emit_signal = ratio_ != ratio;
    if (emit_signal) {
        ratio_ = ratio;
        ratio_changed(ratio);
        value_changed(this->value());
    }
}

Slider_logic::Value_t Slider_logic::value() const {
    return minimum_ +
           std::floor(detail::ceil_if_nearly_whole(ratio_ * this->length()));
}

}  // namespace detail
}  // namespace cppurses
