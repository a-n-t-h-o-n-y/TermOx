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

[[nodiscard]] constexpr auto operator==(Color x, Color y) -> bool
{
    return x.value == y.value;
}

[[nodiscard]] constexpr auto operator!=(Color x, Color y) -> bool
{
    return !(x == y);
}

[[nodiscard]] constexpr auto operator<(Color x, Color y) -> bool
{
    return x.value < y.value;
}

struct Background_color {
    Color::Value_t value;
};

struct Foreground_color {
    Color::Value_t value;
};

/// Converts a Color into a Background_color to be used by Brush.
[[nodiscard]] constexpr auto bg(Color c) -> Background_color
{
    return {c.value};
}

/// Converts a Color into a Background_color to be used by Brush.
[[nodiscard]] constexpr auto fg(Color c) -> Foreground_color
{
    return {c.value};
}

/* --------------------------- Color_index -----------------------------------*/

/// Color Index for XTerm like palette [0 - 255].
using Color_index = ::esc::Color_index;

/* ------------------------------- True Color --------------------------------*/

/// Holds Red, Green, and Blue values; valid range of [0-255] for each color.
using RGB = ::esc::RGB;

/// Holds Hue, Saturation, Lightness values of a color.
using HSL = ::esc::HSL;

/// Holds data for a terminal 'true color'.
/** True colors can be used to set an exact color to the terminal screen. */
using True_color = ::esc::True_color;

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
    // TODO can you get rid of this and use aggregate init? with variant?
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
