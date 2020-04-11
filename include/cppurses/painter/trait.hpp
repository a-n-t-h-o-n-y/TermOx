#ifndef CPPURSES_PAINTER_TRAIT_HPP
#define CPPURSES_PAINTER_TRAIT_HPP
#include <array>

namespace cppurses {

/// Attributes that can be applied to alter the appearance of a Glyph object.
enum class Trait : unsigned char {
    Bold,
    Italic,
    Underline,
    Standout,
    Dim,
    Inverse,
    Invisible,
    Blink
};

/// Number of Traits in enum, use to iterate from [0, Trait_count).
auto constexpr Trait_count = 8;

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_TRAIT_HPP
