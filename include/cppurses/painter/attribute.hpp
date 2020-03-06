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

/// Global list of all Attributes.
/** Useful if querying a Brush for each Attribute with the
 *  Brush::has_attribute() function. Brush holds a std::bitset internally making
 *  a query about a specific Attribute quick. Returning a list of all set
 *  Attributes is expensive, requiring an allocation if using std::vector. Might
 *  change in the future to give a better interface. */
auto constexpr Attribute_list = std::array<Attribute, 8>{
    Attribute::Bold,      Attribute::Italic, Attribute::Underline,
    Attribute::Standout,  Attribute::Dim,    Attribute::Inverse,
    Attribute::Invisible, Attribute::Blink};

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_ATTRIBUTE_HPP
