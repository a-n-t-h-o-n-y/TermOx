#ifndef TERMOX_PAINTER_GLYPH_HPP
#define TERMOX_PAINTER_GLYPH_HPP
#include <utility>

#include <termox/painter/brush.hpp>

namespace ox {

/// Holds a description of a paintable tile on the screen.
/** sizeof(Glyph) == 8 Bytes && alignof(Glyph) == 4 Bytes. */
struct Glyph {
   public:
    /// The Glyph's symbol is the wide character that will be displayed.
    char32_t symbol = U'\0';

    /// The Brush that will determine the Traits and Colors of the symbol.
    Brush brush{};

   public:
    /// Construct an invisible Glyph, defaults to space and no traits/colors.
    constexpr Glyph() = default;

    /// Construct a Glyph with the provided char32_t and Brush.
    constexpr Glyph(char32_t sym, Brush b) : symbol{sym}, brush{b} {}

    /// Construct with the provided char32_t and list of Traits and Colors.
    template <typename... Items>
    constexpr Glyph(char32_t sym, Items... items) : symbol{sym}, brush{items...}
    {}
    // TODO make char constructor
};

// Trait -------------------------------------------------------------------
constexpr auto operator|(Glyph& g, Traits t) -> Glyph&
{
    g.brush.traits |= t;
    return g;
}

constexpr auto operator|(Glyph&& g, Traits t) -> Glyph { return g | t; }

}  // namespace ox

namespace esc {  // For ADL; Trait(s) is really in namespace::esc.

constexpr auto operator|(char32_t g, Trait t) -> ox::Glyph
{
    return ox::Glyph{g} | t;
}

constexpr auto operator|(char32_t g, Traits t) -> ox::Glyph
{
    return ox::Glyph{g} | t;
}

// TODO enable once constructor
// constexpr auto operator|(char g, Trait t) -> ox::Glyph { return ox::Glyph{g}
// | t; }

// constexpr auto operator|(char g, Traits t) -> ox::Glyph { return ox::Glyph{g}
// | t; }

}  // namespace esc

namespace ox {

// Background_color ------------------------------------------------------------
constexpr auto operator|(Glyph& g, Background_color c) -> Glyph&
{
    g.brush.background = Color{c.value};
    return g;
}

constexpr auto operator|(Glyph&& g, Background_color c) -> Glyph
{
    return g | c;
}

constexpr auto operator|(char32_t g, Background_color c) -> Glyph
{
    return Glyph{g} | c;
}

// Foreground_color ------------------------------------------------------------
constexpr auto operator|(Glyph& g, Foreground_color c) -> Glyph&
{
    g.brush.foreground = Color{c.value};
    return g;
}

constexpr auto operator|(Glyph&& g, Foreground_color c) -> Glyph
{
    return g | c;
}

constexpr auto operator|(char32_t g, Foreground_color c) -> Glyph
{
    return Glyph{g} | c;
}

/// Compares symbol and brush for equality.
constexpr auto operator==(Glyph lhs, Glyph rhs) -> bool
{
    return (lhs.symbol == rhs.symbol) and (lhs.brush == rhs.brush);
}

/// Compares symbol and brush for inequality.
constexpr auto operator!=(Glyph lhs, Glyph rhs) -> bool
{
    return !(lhs == rhs);
}

}  // namespace ox
#endif  // TERMOX_PAINTER_GLYPH_HPP
