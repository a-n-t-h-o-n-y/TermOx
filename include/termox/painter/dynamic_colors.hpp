#ifndef TERMOX_PAINTER_DYNAMIC_COLORS_HPP
#define TERMOX_PAINTER_DYNAMIC_COLORS_HPP
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <random>

#include <termox/painter/color.hpp>
#include <termox/system/system.hpp>

namespace ox::dynamic {

/// Dynamic_color type that cycles through every hue value in HSL color wheel.
class Rainbow {
   public:
    /// Creates a Rainbow Dynamic Color w/fixed saturation and lightness values.
    Rainbow(std::uint8_t saturation, std::uint8_t lightness)
        : saturation_{saturation}, lightness_{lightness}
    {}

   public:
    auto operator()() -> True_color
    {
        return True_color{
            HSL{this->postincrement_hue(), saturation_, lightness_}};
    }

   private:
    std::uint16_t hue_ = 0;
    std::uint8_t const saturation_;
    std::uint8_t const lightness_;

   private:
    // Increments hue_, then returns the previous value, wraps at 360.
    auto postincrement_hue() -> std::uint16_t
    {
        auto const old = hue_;
        if (++hue_ == 360)
            hue_ = 0;
        return old;
    }
};

/// Returns a Rainbow Dynamic_color object. Convinience for defining palettes.
inline auto rainbow(
    Dynamic_color::Period_t period = std::chrono::milliseconds{40},
    std::uint8_t saturation        = 75,
    std::uint8_t lightness         = 75) -> Dynamic_color
{
    return {period, Rainbow{saturation, lightness}};
}

/* ---------------------------------------------------------------------------*/

class Modulation_base {
   protected:
    /// Resolution is the number of steps to complete a full cycle.
    Modulation_base(unsigned resolution)
        : step_total_{static_cast<double>(resolution)}
    {}

   protected:
    /// Finds the next ratio between from the current step and resolution.
    auto get_next_ratio() -> double
    {
        return this->post_increment_step() / step_total_;
    }

   private:
    double const step_total_;
    int step_{0};

   private:
    /// Apply a wrapping post increment to step_ and return the previous value.
    auto post_increment_step() -> int
    {
        return step_ == step_total_ ? (step_ = 0, step_total_) : step_++;
    }
};

template <typename Function>
class Invert : public Function {
   public:
    using Function::Function;

   public:
    auto operator()() -> double { return -1. * Function::operator()() + 1.; }
};

template <typename Function>
class Concave : public Function {
   public:
    using Function::Function;

   public:
    auto operator()() -> double { return std::pow(Function::operator()(), 2.); }
};

template <typename Function>
class Convex : public Function {
   public:
    using Function::Function;

   public:
    auto operator()() -> double { return std::sqrt(Function::operator()()); }
};

class Triangle : private Modulation_base {
   public:
    /// Resolution is the number of steps to complete a full cycle.
    Triangle(unsigned resolution) : Modulation_base{resolution} {}

   public:
    /// Returns value in range [0.0, 1.0]
    auto operator()() -> double
    {
        return -1. * std::abs(2 * this->get_next_ratio() - 1.) + 1.;
    }
};

class Sine : private Modulation_base {
   public:
    /// Resolution is the number of steps to complete a full cycle.
    Sine(unsigned resolution) : Modulation_base{resolution} {}

   public:
    /// Returns value in range [0.0, 1.0]
    auto operator()() -> double
    {
        auto constexpr pi = 3.1416;
        return .5 * (1 + std::sin(2 * pi * (this->get_next_ratio() - .25)));
    }
};

class Sawtooth_up : private Modulation_base {
   public:
    /// Resolution is the number of steps to complete a full cycle.
    Sawtooth_up(unsigned resolution) : Modulation_base{resolution} {}

   public:
    /// Returns value in range [0.0, 1.0]
    auto operator()() -> double { return this->get_next_ratio(); }
};

class Sawtooth_down : public Invert<Sawtooth_up> {
   public:
    using Invert::Invert;
};

class Square : private Modulation_base {
   public:
    /// Resolution is the number of steps to complete a full cycle.
    Square(unsigned resolution) : Modulation_base{resolution} {}

   public:
    /// Returns value in range [0.0, 1.0]
    auto operator()() -> double
    {
        return this->get_next_ratio() < 0.5 ? 0. : 1.;
    }
};

class Random : private Modulation_base {
   public:
    /// Resolution is the number of steps to complete a full cycle.
    Random(unsigned resolution) : Modulation_base{resolution} {}

   public:
    /// Returns value in range [0.0, 1.0]
    auto operator()() -> double { return dist_(gen_); }

   private:
    std::mt19937 gen_{std::random_device{}()};
    std::uniform_real_distribution<double> dist_{0., 1.};
};

/// Dynamic_color type that fades between two HSL Colors.
/** Modulation of the shift between the two colors controlled by Shape fn. */
template <typename Shape>
class Fade {
   public:
    /// Dynamic Color that cycles between \p a and \p b and back.
    /** Shape function object should return ratio of where between the two
     *  colors the fade is at. */
    Fade(HSL a, HSL b, Shape&& shape_fn)
        : base_{a}, distance_{find_distance(a, b)}, shape_fn_{shape_fn}
    {}

   public:
    auto operator()() -> True_color { return this->step(); }

   private:
    /// Holds the difference between two HSL values.
    struct HSL_diff {
        int hue_diff;
        int saturation_diff;
        int lightness_diff;
    };

   private:
    HSL const base_;
    HSL_diff const distance_;
    Shape shape_fn_;

   private:
    /// Return HSL_diff composed of the distance between \p a and \p b.
    static auto find_distance(HSL a, HSL b) -> HSL_diff
    {
        return {b.hue - a.hue, b.saturation - a.saturation,
                b.lightness - a.lightness};
    }

    /// Performs a single step and returns the calculated HSL value.
    auto step() -> HSL
    {
        double const ratio = shape_fn_();

        double const h = base_.hue + distance_.hue_diff * ratio;
        double const s = base_.saturation + distance_.saturation_diff * ratio;
        double const l = base_.lightness + distance_.lightness_diff * ratio;

        return this->shrink(h, s, l);
    }

    // Use to create an HSL from double values, performs narrowing conversions.
    static auto shrink(double hue, double saturation, double lightness) -> HSL
    {
        return {static_cast<std::uint16_t>(hue),
                static_cast<std::uint8_t>(saturation),
                static_cast<std::uint8_t>(lightness)};
    }
};

/// Returns a Fade Dynamic_color object. Convinience for defining palettes.
template <typename Shape>
auto fade(HSL a,
          HSL b,
          unsigned resolution              = 400,
          Dynamic_color::Period_t interval = std::chrono::milliseconds{40})
    -> Dynamic_color
{
    return {interval, Fade{a, b, Shape{resolution}}};
}

/// Returns a Fade Dynamic_color object. Convinience for defining palettes.
template <typename Shape>
auto fade(True_color a,
          True_color b,
          unsigned resolution              = 400,
          Dynamic_color::Period_t interval = std::chrono::milliseconds{40})
    -> Dynamic_color
{
    return fade<Shape>(True_color::rgb_to_hsl({a.red(), a.green(), a.blue()}),
                       True_color::rgb_to_hsl({b.red(), b.green(), b.blue()}),
                       resolution, interval);
}

}  // namespace ox::dynamic
#endif  // TERMOX_PAINTER_DYNAMIC_COLORS_HPP
