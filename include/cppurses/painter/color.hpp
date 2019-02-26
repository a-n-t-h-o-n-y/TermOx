#ifndef CPPURSES_PAINTER_COLOR_HPP
#define CPPURSES_PAINTER_COLOR_HPP
#include <cstdint>
#include <string>

namespace cppurses {

using Underlying_color_t = std::int16_t;

namespace detail {
const Underlying_color_t first_color_value{239};
const Underlying_color_t last_color_value{first_color_value + 16};
}  // namespace detail

/// Colors that can be applied to Glyphs.
enum class Color : Underlying_color_t {
    Black = detail::first_color_value,
    Dark_red,
    Dark_blue,
    Dark_gray,
    Brown,
    Green,
    Red,
    Gray,
    Blue,
    Orange,
    Light_gray,
    Light_green,
    Violet,
    Light_blue,
    Yellow,
    White
};

namespace detail {

// Used by add_attributes() in brush to overload on different Color types from a
// parameter pack.
enum class BackgroundColor : Underlying_color_t {
    Black = detail::first_color_value,
    Dark_red,
    Dark_blue,
    Dark_gray,
    Brown,
    Green,
    Red,
    Gray,
    Blue,
    Orange,
    Light_gray,
    Light_green,
    Violet,
    Light_blue,
    Yellow,
    White
};

// Used by add_attributes() in brush to overload on different Color types from a
// parameter pack.
enum class ForegroundColor : Underlying_color_t {
    Black = detail::first_color_value,
    Dark_red,
    Dark_blue,
    Dark_gray,
    Brown,
    Green,
    Red,
    Gray,
    Blue,
    Orange,
    Light_gray,
    Light_green,
    Violet,
    Light_blue,
    Yellow,
    White
};

}  // namespace detail

/// Converts a Color into a detail::BackgroundColor to be used by Brush.
inline constexpr detail::BackgroundColor background(Color c) {
    return static_cast<detail::BackgroundColor>(c);
}

/// Converts a Color into a detail::BackgroundColor to be used by Brush.
inline constexpr detail::ForegroundColor foreground(Color c) {
    return static_cast<detail::ForegroundColor>(c);
}

/// Get the name of the color as a string.
std::string color_to_string(Color c);

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_COLOR_HPP
