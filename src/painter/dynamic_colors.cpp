#include <termox/painter/dynamic_colors.hpp>

#include <cmath>
#include <cstdint>

namespace {

/// Increment x, wrapping to zero at \p limit, returns previous value of x.
template <std::uint16_t Limit>
[[nodiscard]] constexpr auto wrapped_post_increment(std::uint16_t& x)
    -> std::uint16_t
{
    auto const previous = x;
    if (++x == Limit)
        x = 0;
    return previous;
}

}  // namespace

namespace ox::dynamic {

Rainbow::Rainbow(std::uint8_t const saturation, std::uint8_t const lightness)
    : saturation_{saturation}, lightness_{lightness}
{}

auto Rainbow::operator()() -> True_color
{
    return {HSL{wrapped_post_increment<360>(hue_), saturation_, lightness_}};
}

auto rainbow(Dynamic_color::Period_t const period,
             std::uint8_t const saturation,
             std::uint8_t const lightness) -> Dynamic_color
{
    return {period, Rainbow{saturation, lightness}};
}

Modulation_base::Modulation_base(unsigned const resolution)
    : step_total_{resolution}
{}

auto Modulation_base::get_next_ratio() -> double
{
    return this->post_increment_step() / (double)step_total_;
}

auto Modulation_base::post_increment_step() -> int
{
    return (step_ == step_total_) ? (step_ = 0, step_total_) : step_++;
}

Triangle::Triangle(unsigned const resolution) : Modulation_base{resolution} {}

auto Triangle::operator()() -> double
{
    return -1. * std::abs(2 * this->get_next_ratio() - 1.) + 1.;
}

Sine::Sine(unsigned const resolution) : Modulation_base{resolution} {}

auto Sine::operator()() -> double
{
    constexpr auto pi = 3.1416;
    return .5 * (1 + std::sin(2 * pi * (this->get_next_ratio() - .25)));
}

Sawtooth_up::Sawtooth_up(unsigned const resolution)
    : Modulation_base{resolution}
{}

auto Sawtooth_up::operator()() -> double { return this->get_next_ratio(); }

Square::Square(unsigned const resolution) : Modulation_base{resolution} {}

auto Square::operator()() -> double
{
    return (this->get_next_ratio() < 0.5) ? 0. : 1.;
}

Random::Random(unsigned const resolution) : Modulation_base{resolution} {}

auto Random::operator()() -> double { return dist_(gen_); }

}  // namespace ox::dynamic
