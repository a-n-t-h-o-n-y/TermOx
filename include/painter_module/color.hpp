#ifndef COLOR_HPP
#define COLOR_HPP

namespace twf {

enum class Color {
    Black,
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

enum class BackgroundColor {
    Black,
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

enum class ForegroundColor {
    Black,
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

// Free Functions
inline detail::BackgroundColor background(Color c) {
    return static_cast<detail::BackgroundColor>(c);
}

inline detail::ForegroundColor foreground(Color c) {
    return static_cast<detail::ForegroundColor>(c);
}

}  // namespace twf
#endif  // COLOR_HPP
