#include <termox/painter/dynamic_colors.hpp>

#include <cmath>
#include <cstdint>

namespace {

/// Increment x, wrapping to zero at \p limit, returns previous value of x.
auto wrapped_postincrement(std::uint16_t& x, std::uint16_t limit)
    -> std::uint16_t
{
    auto const old = x;
    if (++x == limit)
        x = 0;
    return old;
}

}  // namespace

namespace ox::dynamic {

Rainbow::Rainbow(std::uint8_t saturation, std::uint8_t lightness)
    : saturation_{saturation}, lightness_{lightness}
{}

auto Rainbow::operator()() -> True_color
{
    return {HSL{wrapped_postincrement(hue_, 360), saturation_, lightness_}};
}

auto rainbow(Dynamic_color::Period_t period,
             std::uint8_t saturation,
             std::uint8_t lightness) -> Dynamic_color
{
    return {period, Rainbow{saturation, lightness}};
}

Modulation_base::Modulation_base(unsigned resolution)
    : step_total_{static_cast<double>(resolution)}
{}

auto Modulation_base::get_next_ratio() -> double
{
    return this->post_increment_step() / step_total_;
}

auto Modulation_base::post_increment_step() -> int
{
    return step_ == step_total_ ? (step_ = 0, step_total_) : step_++;
}

Triangle::Triangle(unsigned resolution) : Modulation_base{resolution} {}

auto Triangle::operator()() -> double
{
    return -1. * std::abs(2 * this->get_next_ratio() - 1.) + 1.;
}

Sine::Sine(unsigned resolution) : Modulation_base{resolution} {}

auto Sine::operator()() -> double
{
    auto constexpr pi = 3.1416;
    return .5 * (1 + std::sin(2 * pi * (this->get_next_ratio() - .25)));
}

Sawtooth_up::Sawtooth_up(unsigned resolution) : Modulation_base{resolution} {}

auto Sawtooth_up::operator()() -> double { return this->get_next_ratio(); }

Square::Square(unsigned resolution) : Modulation_base{resolution} {}

auto Square::operator()() -> double
{
    return this->get_next_ratio() < 0.5 ? 0. : 1.;
}

Random::Random(unsigned resolution) : Modulation_base{resolution} {}

auto Random::operator()() -> double { return dist_(gen_); }

}  // namespace ox::dynamic
