#ifndef CPPURSES_PAINTER_RGB_HPP
#define CPPURSES_PAINTER_RGB_HPP
#include <cstdint>

#include <cppurses/painter/color.hpp>

namespace cppurses {

    // TODO Remove
/// Holds red, green, and blue values, assumed to be [0-256) in value.
struct RGB {
    using Value_t = std::uint8_t;
    Value_t red;
    Value_t green;
    Value_t blue;
};

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_RGB_HPP
