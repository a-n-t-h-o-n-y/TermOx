#ifndef CPPURSES_PAINTER_PALETTES_HPP
#define CPPURSES_PAINTER_PALETTES_HPP
#include <cppurses/painter/palette.hpp>

namespace cppurses {

/// A collection of pre-defined Palettes from the library.
struct Palettes {
    static Palette Standard();
    static Palette DawnBringer();
};

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_PALETTES_HPP
