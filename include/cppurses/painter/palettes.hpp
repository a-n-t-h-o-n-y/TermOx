#ifndef CPPURSES_PAINTER_PALETTES_HPP
#define CPPURSES_PAINTER_PALETTES_HPP
#include <cppurses/painter/palette.hpp>

namespace cppurses {

/// A collection of pre-defined Palettes from the library.
struct Palettes {
    static auto Standard() -> Palette;
    static auto DawnBringer() -> Palette;
};

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_PALETTES_HPP
