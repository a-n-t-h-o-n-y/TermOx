#ifndef CPPURSES_PAINTER_RGB_HPP
#define CPPURSES_PAINTER_RGB_HPP
#include <cppurses/painter/color.hpp>

namespace cppurses {

/// Holds red, green, and blue values, assumed to be [0-256) in value.
struct RGB {
    Underlying_color_t red;
    Underlying_color_t green;
    Underlying_color_t blue;
};

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_RGB_HPP
