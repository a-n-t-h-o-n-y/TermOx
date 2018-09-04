#ifndef CPPURSES_PAINTER_COLOR_DEFINITION_HPP
#define CPPURSES_PAINTER_COLOR_DEFINITION_HPP
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/rgb.hpp>

namespace cppurses {

/// Holds RGB values for a given Color, using values [0, 255].
struct Color_definition {
    Color color;
    RGB values;
};

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_COLOR_DEFINITION_HPP

