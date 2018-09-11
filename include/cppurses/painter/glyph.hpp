#ifndef CPPURSES_PAINTER_GLYPH_HPP
#define CPPURSES_PAINTER_GLYPH_HPP
#include <utility>

#include <cppurses/painter/brush.hpp>

namespace cppurses {

/// Holds a description of a paintable tile on the screen.
struct Glyph {
    /// Construct an invisible Glyph, defaults to space and no attrs/colors.
    Glyph() = default;

    /// Construct a Glyph with the provided wchar_t and Brush.
    Glyph(wchar_t sym, const Brush& b);

    /// Construct with the provided wchar_t and list of Attributes and Colors.
    template <typename... Attributes>
    Glyph(wchar_t sym, Attributes&&... attrs);

    /// The Glyph's symbol is the wide character that will be displayed.
    wchar_t symbol{L' '};

    /// The Brush that will determine the Attributes and Colors of the symbol.
    Brush brush;
};

/// Compares if each symbol and brush are equal.
bool operator==(const Glyph& lhs, const Glyph& rhs);

/// Compares if each symbol and brush are not equal.
bool operator!=(const Glyph& lhs, const Glyph& rhs);

// TEMPLATE IMPLEMENTATIONS
template <typename... Attributes>
Glyph::Glyph(wchar_t sym, Attributes&&... attrs)
    : symbol{sym}, brush{std::forward<Attributes>(attrs)...} {}

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_GLYPH_HPP
