#ifndef TERMOX_PAINTER_DYNAMIC_COLORS_HPP
#define TERMOX_PAINTER_DYNAMIC_COLORS_HPP
#include <chrono>
#include <cmath>
#include <cstdint>
#include <random>
#include <utility>

#include <termox/painter/color.hpp>

namespace ox::dynamic {

/// Dynamic_color type that cycles through every hue value in HSL color wheel.
class Rainbow {
   public:
    /// Creates a Rainbow Dynamic Color w/fixed saturation and lightness values.
    Rainbow(std::uint8_t saturation, std::uint8_t lightness);

   public:
    /// Increment and return the next True_color.
    [[nodiscard]] auto operator()() -> True_color;

   private:
    std::uint16_t hue_ = 0;
    std::uint8_t const saturation_;
    std::uint8_t const lightness_;
};

/// Returns a Rainbow Dynamic_color object. Convinience for defining palettes.
[[nodiscard]] auto rainbow(
    Dynamic_color::Period_t period = std::chrono::milliseconds{40},
    std::uint8_t saturation        = 75,
    std::uint8_t lightness         = 75) -> Dynamic_color;

/* ---------------------------------------------------------------------------*/

class Modulation_base {
   protected:
    /// Resolution is the number of steps to complete a full cycle.
    /** \p offset is the starting point, in range [0, resolution). */
    explicit Modulation_base(unsigned resolution, unsigned offset = 0);

   protected:
    /// Finds the next ratio between from the current step and resolution.
    /** Returns a value between [0.0, 1.0]. */
    [[nodiscard]] auto get_next_ratio() -> double;

   private:
    unsigned const step_total_;
    unsigned step_;

   private:
    /// Apply a wrapping post increment to step_ and return the previous value.
    [[nodiscard]] auto post_increment_step() -> int;
};

template <typename Function>
class Invert : public Function {
   public:
    using Function::Function;

   public:
    [[nodiscard]] auto operator()() -> double
    {
        return -1. * Function::operator()() + 1.;
    }
};

template <typename Function>
class Concave : public Function {
   public:
    using Function::Function;

   public:
    [[nodiscard]] auto operator()() -> double
    {
        return std::pow(Function::operator()(), 2.);
    }
};

template <typename Function>
class Convex : public Function {
   public:
    using Function::Function;

   public:
    [[nodiscard]] auto operator()() -> double
    {
        return std::sqrt(Function::operator()());
    }
};

class Triangle : private Modulation_base {
   public:
    /// Resolution is the number of steps to complete a full cycle.
    Triangle(unsigned resolution);

   public:
    /// Returns value in range [0.0, 1.0]
    [[nodiscard]] auto operator()() -> double;
};

class Sine : private Modulation_base {
   public:
    /// Resolution is the number of steps to complete a full cycle.
    Sine(unsigned resolution);

   public:
    /// Returns value in range [0.0, 1.0]
    [[nodiscard]] auto operator()() -> double;
};

class Sawtooth_up : private Modulation_base {
   public:
    /// Resolution is the number of steps to complete a full cycle.
    Sawtooth_up(unsigned resolution);

   public:
    /// Returns value in range [0.0, 1.0]
    [[nodiscard]] auto operator()() -> double;
};

class Sawtooth_down : public Invert<Sawtooth_up> {
   public:
    using Invert::Invert;
};

class Square : private Modulation_base {
   public:
    /// Resolution is the number of steps to complete a full cycle.
    Square(unsigned resolution);

   public:
    /// Returns value in range [0.0, 1.0]
    [[nodiscard]] auto operator()() -> double;
};

class Random : private Modulation_base {
   public:
    /// Resolution is the number of steps to complete a full cycle.
    Random(unsigned resolution);

   public:
    /// Returns value in range [0.0, 1.0]
    [[nodiscard]] auto operator()() -> double;

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
    Fade(HSL const a, HSL const b, Shape&& shape_fn)
        : base_{a},
          distance_{find_distance(a, b)},
          shape_fn_{std::forward<Shape>(shape_fn)}
    {}

   public:
    [[nodiscard]] auto operator()() -> True_color { return this->step(); }

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
    /// Performs a single step and returns the calculated HSL value.
    [[nodiscard]] auto step() -> HSL
    {
        double const ratio = shape_fn_();

        double const h = base_.hue + distance_.hue_diff * ratio;
        double const s = base_.saturation + distance_.saturation_diff * ratio;
        double const l = base_.lightness + distance_.lightness_diff * ratio;

        return narrow(h, s, l);
    }

   private:
    /// Return HSL_diff composed of the distance between \p a and \p b.
    [[nodiscard]] static auto find_distance(HSL const a, HSL const b)
        -> HSL_diff
    {
        return {b.hue - a.hue, b.saturation - a.saturation,
                b.lightness - a.lightness};
    }

    /// Use to create an HSL from double values, performs narrowing conversions.
    [[nodiscard]] static auto narrow(double const hue,
                                     double const saturation,
                                     double const lightness) -> HSL
    {
        return {static_cast<std::uint16_t>(hue),
                static_cast<std::uint8_t>(saturation),
                static_cast<std::uint8_t>(lightness)};
    }
};

/// Returns a Fade Dynamic_color object. Convinience for defining palettes.
template <typename Shape>
[[nodiscard]] auto fade(HSL const a,
                        HSL const b,
                        unsigned const resolution = 400,
                        Dynamic_color::Period_t const interval =
                            std::chrono::milliseconds{40}) -> Dynamic_color
{
    return {interval, Fade{a, b, Shape{resolution}}};
}

/// Returns a Fade Dynamic_color object. Convinience for defining palettes.
template <typename Shape>
[[nodiscard]] auto fade(True_color const a,
                        True_color const b,
                        unsigned const resolution = 400,
                        Dynamic_color::Period_t const interval =
                            std::chrono::milliseconds{40}) -> Dynamic_color
{
    return fade<Shape>(esc::rgb_to_hsl({a.red, a.green, a.blue}),
                       esc::rgb_to_hsl({b.red, b.green, b.blue}), resolution,
                       interval);
}

}  // namespace ox::dynamic
#endif  // TERMOX_PAINTER_DYNAMIC_COLORS_HPP
