#ifndef CPPURSES_PAINTER_COLOR_HPP
#define CPPURSES_PAINTER_COLOR_HPP
#include <chrono>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace cppurses {

/// Color numbers [0 - 15] TODO add up to 36? requires 1296 color pairs
struct Color {
    using Value_t = std::uint8_t;
    Value_t value;

    // Default Color Names
    static Color const Black;
    static Color const Dark_red;
    static Color const Green;
    static Color const Brown;
    static Color const Dark_blue;
    static Color const Violet;
    static Color const Light_blue;
    static Color const Light_gray;
    static Color const Dark_gray;
    static Color const Red;
    static Color const Light_green;
    static Color const Yellow;
    static Color const Blue;
    static Color const Orange;
    static Color const Gray;
    static Color const White;
};

constexpr auto operator==(Color x, Color y) -> bool
{
    return x.value == y.value;
}

constexpr auto operator!=(Color x, Color y) -> bool { return !(x == y); }

struct Background_color {
    Color::Value_t value;
};

struct Foreground_color {
    Color::Value_t value;
};

/// Converts a Color into a Background_color to be used by Brush.
constexpr auto background(Color c) -> Background_color { return {c.value}; }

/// Converts a Color into a Background_color to be used by Brush.
constexpr auto foreground(Color c) -> Foreground_color { return {c.value}; }

/* --------------------------- ANSI/XTERM Color ------------------------------*/

/// ANSI color numbers [0 - 255].
struct ANSI {
    using Value_t = std::uint8_t;
    Value_t value;
};

/* ------------------------------- True Color --------------------------------*/

/// Holds Red, Green, and Blue values, valid range of [0-255].
struct RGB {
    using Value_t = std::uint8_t;
    Value_t red;
    Value_t green;
    Value_t blue;
};

/// Holds Hue, Saturation, Lightness values of a color.
struct HSL {
    std::uint16_t hue;        // [0, 359] degrees
    std::uint8_t saturation;  // [0, 100] %
    std::uint8_t lightness;   // [0, 100] %
};

/// Holds a True Color definition in terms of RGB values.
class True_color {
   public:
    using Value_t = std::uint32_t;

   public:
    constexpr True_color(Value_t hex) : value_{hex} {}

    constexpr True_color(RGB x) : value_{merge(x)} {}

    constexpr True_color(HSL x) : value_{merge(hsl_to_rgb(x))} {}

   public:
    /// Returns the red component value [0, 255]
    constexpr auto red() const -> std::uint8_t { return (value_ >> 16) & 0xFF; }

    /// Returns the green component value [0, 255]
    constexpr auto green() const -> std::uint8_t
    {
        return (value_ >> 8) & 0xFF;
    }

    /// Returns the blue component value [0, 255]
    constexpr auto blue() const -> std::uint8_t { return value_ & 0xFF; }

   private:
    Value_t value_;

   private:
    /// Merge RGB values into single 24 bit value
    static constexpr auto merge(RGB x) -> Value_t
    {
        return (x.red << 16) + (x.green << 8) + x.blue;
    }

    template <typename T>
    static constexpr auto abs(T x) -> T
    {
        return (x == T(0) ? T(0) : x < T(0) ? -x : x);
    }

    template <typename T>
    static constexpr auto fmod(const T x, const T y) -> T
    {
        return (x - T(static_cast<long long>(x / y)) * y);
    }

    static constexpr auto hsl_to_rgb(HSL v) -> RGB
    {
        double const lightness  = v.lightness / 100.;
        double const saturation = v.saturation / 100.;

        auto const c         = (1 - abs((2 * lightness) - 1.)) * saturation;
        double const h_prime = v.hue / 60.;
        double const x       = c * (1. - abs(fmod(h_prime, 2.) - 1.));
        double const m       = lightness - (c / 2.);

        auto const c_ = static_cast<RGB::Value_t>((c + m) * 255);
        auto const x_ = static_cast<RGB::Value_t>((x + m) * 255);
        auto const m_ = static_cast<RGB::Value_t>(m * 255);

        if (v.hue < 60.)
            return {c_, x_, m_};
        if (v.hue < 120.)
            return {x_, c_, m_};
        if (v.hue < 180.)
            return {m_, c_, x_};
        if (v.hue < 240.)
            return {m_, x_, c_};
        if (v.hue < 300.)
            return {x_, m_, c_};
        if (v.hue < 360.)
            return {c_, m_, x_};
        else
            return {0, 0, 0};
    }
};

/// Returns a True_color from RGB values. Convinience for defining palettes.
inline constexpr auto rgb(RGB::Value_t r, RGB::Value_t g, RGB::Value_t b)
    -> True_color
{
    return {RGB{r, g, b}};
}

/// Returns a True_color from HSL values. Convinience for defining palettes.
inline constexpr auto hsl(std::uint16_t h, std::uint8_t s, std::uint8_t l)
    -> True_color
{
    return {HSL{h, s, l}};
}

/* ----------------------------- Dynamic Color -------------------------------*/

/// Defines an animated color
/** get_value() is called every period and used to update the color. */
struct Dynamic_color {
    using Period_t = std::chrono::milliseconds;
    Period_t period;
    std::function<True_color()> get_value;
};

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
        return {HSL{this->postincrement_hue(), saturation_, lightness_}};
    }

   private:
    std::uint16_t hue_ = 0;
    std::uint8_t saturation_;
    std::uint8_t lightness_;

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
    std::uint8_t saturation        = 50,
    std::uint8_t lightness         = 50) -> Dynamic_color
{
    return {period, Rainbow{saturation, lightness}};
}

/* ----------------------------- Color Palette -------------------------------*/

/// Used to define a single color for a Color_palette.
class Color_definition {
   public:
    using Value_t = std::variant<std::monostate, True_color, Dynamic_color>;

   public:
    Color color;
    ANSI ansi;
    Value_t value;

   public:
    Color_definition(Color c, ANSI a)
        : color{c}, ansi{a}, value{std::monostate{}}
    {}

    Color_definition(Color c, ANSI a, Value_t v)
        : color{c}, ansi{a}, value{std::move(v)}
    {}
};

/// Max size of 181 colors in a palette
/** 181 colors will need 32,761 pairs, max_pairs in ncurses is 32,767. */
using Color_palette = std::vector<Color_definition>;

/* ---------------------------------------------------------------------------*/

/// Get the default name of the color as a string.
inline auto color_to_string(Color c) -> std::string
{
    switch (c.value) {
        case 0: return "Black";
        case 1: return "Red";
        case 2: return "Green";
        case 3: return "Yellow";
        case 4: return "Blue";
        case 5: return "Violet";
        case 6: return "Light Blue";
        case 7: return "White";
        case 8: return "Dark Gray";
        case 9: return "Dark Red";
        case 10: return "Light Green";
        case 11: return "Brown";
        case 12: return "Dark Blue";
        case 13: return "Orange";
        case 14: return "Gray";
        case 15: return "Light Gray";
        default: return "";
    }
}

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_COLOR_HPP
