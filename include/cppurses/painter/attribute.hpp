#ifndef PAINTER_ATTRIBUTE_HPP
#define PAINTER_ATTRIBUTE_HPP
#include <cstdint>

namespace cppurses {

/// Attributes that can be applied to a Glyph.
enum class Attribute : std::int8_t {
    // Update ncurses_paint_engine::put_glyph() if you modify this enum.
    Bold,
    Italic,
    Underline,
    Standout,
    Dim,
    Inverse,
    Invisible,
    Blink
};

}  // namespace cppurses
#endif  // PAINTER_ATTRIBUTE_HPP
