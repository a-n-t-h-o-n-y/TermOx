#ifndef PAINTER_COLOR_HPP
#define PAINTER_COLOR_HPP
#include <cstdint>

namespace cppurses {

namespace detail {
const int k_init_color{239};
}  // namespace detail

enum class Color : std::int16_t {
    Black = detail::k_init_color,
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

enum class BackgroundColor : std::int16_t {
    Black = detail::k_init_color,
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

enum class ForegroundColor : std::int16_t {
    Black = detail::k_init_color,
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

inline detail::BackgroundColor background(Color c) {
    return static_cast<detail::BackgroundColor>(c);
}

inline detail::ForegroundColor foreground(Color c) {
    return static_cast<detail::ForegroundColor>(c);
}

}  // namespace cppurses
#endif  // PAINTER_COLOR_HPP
