#ifndef CPPURSES_PAINTER_COLOR_HPP
#define CPPURSES_PAINTER_COLOR_HPP
#include <cstdint>
#include <stdexcept>
#include <string>
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

/* --------------------------- ANSI Color Palette ----------------------------*/
/// ANSI color numbers [0 - 255].
struct ANSI {
    using Value_t = std::uint8_t;
    Value_t value;
};
inline auto operator==(ANSI x, ANSI y) -> bool { return x.value == y.value; }
inline auto operator!=(ANSI x, ANSI y) -> bool { return !(x == y); }

struct ANSI_definition {
    Color color;
    ANSI ansi;
};

/// Max size of 181 colors in a palette
/** 181 colors will need 32,761 pairs, max_pairs in ncurses is 32,767. */
using ANSI_palette = std::vector<ANSI_definition>;

/* --------------------------- True Color Palette ----------------------------*/
class True_color {
   public:
    using Value_t = std::uint32_t;

   public:
    constexpr True_color(Value_t hex) : value_{hex} {}

    constexpr True_color(std::uint8_t red,
                         std::uint8_t green,
                         std::uint8_t blue)
        : value_{merge(red, green, blue)}
    {}

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
    /// Merge rgb values into single 24 bit value
    static constexpr auto merge(std::uint8_t r, std::uint8_t g, std::uint8_t b)
        -> Value_t
    {
        return (r << 16) + (g << 8) + b;
    }
};

/// Ties an ANSI color and cppurses::Color to a True_color value.
struct True_color_definition {
    ANSI_definition ansi_def;
    True_color color_value;
};

/// Max size of 181 colors in a palette
/** 181 colors will need 32,761 pairs, max_pairs in ncurses is 32,767. */
using True_color_palette = std::vector<True_color_definition>;

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
        default: throw std::logic_error{"color_to_string: Unreachable"};
    }
}

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_COLOR_HPP
