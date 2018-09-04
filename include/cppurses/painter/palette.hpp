#ifndef CPPURSES_PAINTER_PALETTE_HPP
#define CPPURSES_PAINTER_PALETTE_HPP
#include <array>

#include <cppurses/painter/color_definition.hpp>

namespace cppurses {

/// Holds definitions for each of the 16 colors used in the library.
using Palette = std::array<Color_definition, 16>;

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_PALETTE_HPP
