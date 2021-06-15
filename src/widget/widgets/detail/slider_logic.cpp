#include <termox/widget/widgets/detail/slider_logic.hpp>

#include <cmath>
#include <limits>
#include <stdexcept>

#include <termox/widget/widgets/detail/nearly_equal.hpp>

namespace ox::detail {

Slider_logic::Slider_logic(Value_t minimum, Value_t maximum)
{
    this->set_minimum(minimum);
    this->set_maximum(maximum);
    this->set_value(minimum_);
}

void Slider_logic::set_minimum(Value_t min)
{
    if (std::numeric_limits<Value_t>::max() == min)
        throw std::overflow_error{"Slider_logic::set_minimum."};
    if (maximum_ <= min)
        maximum_ = min + 1;
    minimum_ = min;
}

void Slider_logic::set_maximum(Value_t max)
{
    if (std::numeric_limits<Value_t>::min() == max)
        throw std::underflow_error{"Slider_logic::set_maximum."};
    if (minimum_ >= max)
        minimum_ = max - 1;
    maximum_ = max;
}

auto Slider_logic::minimum() const -> Value_t { return minimum_; }

auto Slider_logic::maximum() const -> Value_t { return maximum_; }

void Slider_logic::increment(Value_t amount)
{
    this->set_value(this->value() + amount);
}

void Slider_logic::decrement(Value_t amount)
{
    this->set_value(this->value() - amount);
}

void Slider_logic::set_ratio(Ratio_t ratio)
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

auto Slider_logic::ratio() const -> double { return ratio_; }

void Slider_logic::set_value(Value_t value)
{
    if (value > maximum_)
        value = maximum_;
    else if (value < minimum_)
        value = minimum_;
    auto const ratio = static_cast<double>(value - minimum_) / this->length();
    this->set_ratio(ratio);
}

auto Slider_logic::value() const -> Value_t
{
    return minimum_ + (Value_t)std::floor(detail::ceil_if_nearly_whole(
                          ratio_ * this->length()));
}

auto Slider_logic::length() const -> Value_t { return maximum_ - minimum_; }

}  // namespace ox::detail
