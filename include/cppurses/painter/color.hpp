#ifndef CPPURSES_PAINTER_COLOR_HPP
#define CPPURSES_PAINTER_COLOR_HPP
#include <cstdint>
#include <stdexcept>
#include <string>

namespace cppurses {

using Underlying_color_t = std::int16_t;

namespace detail {

auto constexpr first_color_value = Underlying_color_t{0};
auto constexpr last_color_value  = Underlying_color_t{first_color_value + 16};

}  // namespace detail

/// Colors that can be applied to Glyphs.
enum class Color : Underlying_color_t {
    // Compatibility Colors
    Black = detail::first_color_value,
    Red,
    Green,
    Yellow,
    Blue,
    Violet,
    Light_blue,
    White,

    // Extended Colors
    Dark_gray,
    Dark_red,
    Light_green,
    Brown,
    Dark_blue,
    Orange,
    Gray,
    Light_gray
};

namespace detail {

// Used by add_attributes() in brush to overload on different Color types from a
// parameter pack.
enum class BackgroundColor : Underlying_color_t {
    // Compatibility Colors
    Black = detail::first_color_value,
    Red,
    Green,
    Yellow,
    Blue,
    Violet,
    Light_blue,
    White,

    // Extended Colors
    Dark_gray,
    Dark_red,
    Light_green,
    Brown,
    Dark_blue,
    Orange,
    Gray,
    Light_gray
};

// Used by add_attributes() in brush to overload on different Color types from a
// parameter pack.
enum class ForegroundColor : Underlying_color_t {
    // Compatibility Colors
    Black = detail::first_color_value,
    Red,
    Green,
    Yellow,
    Blue,
    Violet,
    Light_blue,
    White,

    // Extended Colors
    Dark_gray,
    Dark_red,
    Light_green,
    Brown,
    Dark_blue,
    Orange,
    Gray,
    Light_gray
};

}  // namespace detail

/// Converts a Color into a detail::BackgroundColor to be used by Brush.
inline constexpr auto background(Color c) -> detail::BackgroundColor
{
    return static_cast<detail::BackgroundColor>(c);
}

/// Converts a Color into a detail::BackgroundColor to be used by Brush.
inline constexpr auto foreground(Color c) -> detail::ForegroundColor
{
    return static_cast<detail::ForegroundColor>(c);
}

/// Get the name of the color as a string.
inline auto color_to_string(Color c) -> std::string
{
    switch (c) {
        case Color::Black: return "Black";
        case Color::Dark_red: return "Dark Red";
        case Color::Dark_blue: return "Dark Blue";
        case Color::Dark_gray: return "Dark Gray";
        case Color::Brown: return "Brown";
        case Color::Green: return "Green";
        case Color::Red: return "Red";
        case Color::Gray: return "Gray";
        case Color::Blue: return "Blue";
        case Color::Orange: return "Orange";
        case Color::Light_gray: return "Light Gray";
        case Color::Light_green: return "Light Green";
        case Color::Violet: return "Violet";
        case Color::Light_blue: return "Light Blue";
        case Color::Yellow: return "Yellow";
        case Color::White: return "White";
    }
    throw std::logic_error{"Unreachable"};
}

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_COLOR_HPP
