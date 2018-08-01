#ifndef CPPURSES_PAINTER_RGB_HPP
#define CPPURSES_PAINTER_RGB_HPP
#include <cstdint>

namespace cppurses {

/// Represents red, green, and blue values, assumed to be [0-256) in value.
struct RGB {
    std::int16_t red;
    std::int16_t green;
    std::int16_t blue;
};

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_RGB_HPP
