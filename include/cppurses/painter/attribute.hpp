#ifndef CPPURSES_PAINTER_ATTRIBUTE_HPP
#define CPPURSES_PAINTER_ATTRIBUTE_HPP
#include <array>

namespace cppurses {

/// Attributes that can be applied to alter the appearance of a Glyph object.
enum class Attribute : unsigned char {
    Bold,
    Italic,
    Underline,
    Standout,
    Dim,
    Inverse,
    Invisible,
    Blink
};

/// Number of Attributes in enum, use to iterate from [0, Attribute_count).
auto constexpr Attribute_count = 8;

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_ATTRIBUTE_HPP
