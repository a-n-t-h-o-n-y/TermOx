#ifndef TERMOX_PAINTER_COLOR_HPP
#define TERMOX_PAINTER_COLOR_HPP
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include <esc/color_index.hpp>
#include <esc/true_color.hpp>

namespace ox {

/// Color numbers [0 - 180] are valid.
class Color {
   public:
    using Value_t = std::uint8_t;
    Value_t value;

   public:
    enum Name : Value_t {
        Background  = 0,
        Black       = 0,
        Dark_red    = 1,
        Green       = 2,
        Brown       = 3,
        Dark_blue   = 4,
        Violet      = 5,
        Light_blue  = 6,
        Light_gray  = 7,
        Dark_gray   = 8,
        Red         = 9,
        Light_green = 10,
        Yellow      = 11,
        Blue        = 12,
        Orange      = 13,
        Gray        = 14,
        White       = 15,
        Foreground  = 15,
    };

   public:
    constexpr Color(Name n) : value{static_cast<Value_t>(n)} {}
    constexpr explicit Color(Value_t v) : value{v} {}
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
constexpr auto bg(Color c) -> Background_color { return {c.value}; }

/// Converts a Color into a Background_color to be used by Brush.
constexpr auto fg(Color c) -> Foreground_color { return {c.value}; }

/* --------------------------- Color_index -----------------------------------*/

/// Color Index for XTerm like palette [0 - 255].
using Color_index = ::esc::Color_index;

// TODO convert ANSI to Color_index, unless its True_color.
/// ANSI color numbers [0 - 255].
// struct ANSI {
//     using Value_t = std::uint8_t;
//     Value_t value;
// };

// inline auto operator==(ANSI a, ANSI b) -> bool { return a.value == b.value; }
// inline auto operator!=(ANSI a, ANSI b) -> bool { return a.value != b.value; }

/* ------------------------------- True Color --------------------------------*/

/// Holds Red, Green, and Blue values; valid range of [0-255] for each color.
using RGB = ::esc::RGB;

/// Holds Hue, Saturation, Lightness values of a color.
using HSL = ::esc::HSL;

/// Holds data for a terminal 'true color'.
/** True colors can be used to set an exact color to the terminal screen. */
using True_color = ::esc::True_color;

// struct RGB {
//     using Value_t = std::uint8_t;
//     Value_t red;
//     Value_t green;
//     Value_t blue;
// };

/// Holds Hue, Saturation, Lightness values of a color.
// struct HSL {
//     std::uint16_t hue;        // [0, 359] degrees
//     std::uint8_t saturation;  // [0, 100] %
//     std::uint8_t lightness;   // [0, 100] %
// };

/// Holds a True Color definition in terms of RGB values.
// class True_color {
//    public:
//     using Value_t = std::uint32_t;

//    public:
//     constexpr True_color(Value_t hex) : value_{hex} {}

//     constexpr True_color(RGB x) : value_{merge(x)} {}

//     constexpr True_color(HSL x) : value_{merge(hsl_to_rgb(x))} {}

//    public:
//     /// Returns the red component value [0, 255]
//     constexpr auto red() const -> std::uint8_t { return (value_ >> 16) &
//     0xFF; }

//     /// Returns the green component value [0, 255]
//     constexpr auto green() const -> std::uint8_t
//     {
//         return (value_ >> 8) & 0xFF;
//     }

//     /// Returns the blue component value [0, 255]
//     constexpr auto blue() const -> std::uint8_t { return value_ & 0xFF; }

//    private:
//     Value_t value_;

//    private:
//     /// Merge RGB values into single 24 bit value
//     static constexpr auto merge(RGB x) -> Value_t
//     {
//         return (x.red << 16) + (x.green << 8) + x.blue;
//     }

//     template <typename T>
//     static constexpr auto abs(T x) -> T
//     {
//         return (x == T(0) ? T(0) : x < T(0) ? -x : x);
//     }

//     template <typename T>
//     static constexpr auto fmod(const T x, const T y) -> T
//     {
//         return (x - T(static_cast<long long>(x / y)) * y);
//     }

//     static constexpr auto hsl_to_rgb(HSL v) -> RGB
//     {
//         double const lightness  = v.lightness / 100.;
//         double const saturation = v.saturation / 100.;

//         auto const c         = (1 - abs((2 * lightness) - 1.)) * saturation;
//         double const h_prime = v.hue / 60.;
//         double const x       = c * (1. - abs(fmod(h_prime, 2.) - 1.));
//         double const m       = lightness - (c / 2.);

//         auto const c_ = static_cast<RGB::Value_t>((c + m) * 255);
//         auto const x_ = static_cast<RGB::Value_t>((x + m) * 255);
//         auto const m_ = static_cast<RGB::Value_t>(m * 255);

//         if (v.hue < 60.)
//             return {c_, x_, m_};
//         if (v.hue < 120.)
//             return {x_, c_, m_};
//         if (v.hue < 180.)
//             return {m_, c_, x_};
//         if (v.hue < 240.)
//             return {m_, x_, c_};
//         if (v.hue < 300.)
//             return {x_, m_, c_};
//         if (v.hue < 360.)
//             return {c_, m_, x_};
//         else
//             return {0, 0, 0};
//     }

//    public:
//     static [> constexpr <] auto rgb_to_hsl(RGB x) -> HSL
//     {
//         double const r_prime = x.red / 255.;
//         double const g_prime = x.green / 255.;
//         double const b_prime = x.blue / 255.;

//         double const c_max = std::max({r_prime, g_prime, b_prime});
//         double const c_min = std::min({r_prime, g_prime, b_prime});
//         double const delta = c_max - c_min;

//         double const lightness  = (c_max + c_min) / 2.;
//         double const saturation = [&] {
//             if (delta == 0.)
//                 return 0.;
//             double const den = 1. - abs(2. * lightness - 1.);
//             return delta / den;
//         }();
//         std::uint16_t const hue = [&] {
//             if (delta == 0.)
//                 return 0.;
//             if (c_max == r_prime)
//                 return 60. * fmod((g_prime - b_prime) / delta, 6.);
//             if (c_max == g_prime)
//                 return 60. * (((b_prime - r_prime) / delta) + 2.);
//             if (c_max == b_prime)
//                 return 60. * (((r_prime - g_prime) / delta) + 4.);
//             return .0;
//         }();
//         return {static_cast<std::uint16_t>(hue),
//                 static_cast<std::uint8_t>(saturation * 100),
//                 static_cast<std::uint8_t>(lightness * 100)};
//     }
// };

/// Returns a True_color from RGB values. Convinience for defining palettes.
// inline constexpr auto rgb(RGB::Value_t r, RGB::Value_t g, RGB::Value_t b)
//     -> True_color
// {
//     return {RGB{r, g, b}};
// }

// /// Returns a True_color from HSL values. Convinience for defining palettes.
// inline constexpr auto hsl(std::uint16_t h, std::uint8_t s, std::uint8_t l)
//     -> True_color
// {
//     return {HSL{h, s, l}};
// }

/* ----------------------------- Dynamic Color -------------------------------*/

/// Defines an animated color
/** get_value() is called every interval and used to update the color. */
struct Dynamic_color {
    using Period_t = std::chrono::milliseconds;
    Period_t interval;
    std::function<True_color()> get_value;
};

/* ----------------------------- Color Palette -------------------------------*/

/// Used to define a single color for a Palette.
class Color_definition {
   public:
    using Value_t = std::variant<Color_index, True_color, Dynamic_color>;

   public:
    Color color;
    Value_t value;

   public:
    Color_definition(Color c, Value_t v) : color{c}, value{v} {}
};

/// A Color Palette pairs Color values and Color_definitions.
/** After setting a particular palette, all Colors within the Palette will be
 *  associated with the Color_definition it is paired with. */
using Palette = std::vector<Color_definition>;

/// Create a Palette by pairing Color_definition::Value_t with Colors.
/** Colors are paired in order Color_definition::Value_t's are added,
 *  starting with Color{0}. */
template <typename... ColorValues>
[[nodiscard]] static auto make_palette(ColorValues... values) -> Palette
{
    auto c = Color::Value_t{0};
    return {Color_definition{Color{c++}, values}...};
}

/* ---------------------------------------------------------------------------*/

/// Get the default name of the color as a string.
[[nodiscard]] inline auto color_to_string(Color c) -> std::string
{
    switch (c.value) {
        case 0: return "Black";
        case 1: return "Dark Red";
        case 2: return "Green";
        case 3: return "Brown";
        case 4: return "Dark Blue";
        case 5: return "Violet";
        case 6: return "Light Blue";
        case 7: return "Light Gray";
        case 8: return "Dark Gray";
        case 9: return "Red";
        case 10: return "Light Green";
        case 11: return "Yellow";
        case 12: return "Blue";
        case 13: return "Orange";
        case 14: return "Gray";
        case 15: return "White";
        default: return "";
    }
}

}  // namespace ox
#endif  // TERMOX_PAINTER_COLOR_HPP
