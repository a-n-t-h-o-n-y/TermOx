#ifndef CPPURSES_PAINTER_GLYPH_HPP
#define CPPURSES_PAINTER_GLYPH_HPP
#include <utility>

#include <cppurses/painter/brush.hpp>

namespace cppurses {

/// Holds a description of a paintable tile on the screen.
struct Glyph {
   public:
    /// The Glyph's symbol is the wide character that will be displayed.
    wchar_t symbol = L' ';

    /// The Brush that will determine the Traits and Colors of the symbol.
    Brush brush{};

   public:
    /// Construct an invisible Glyph, defaults to space and no traits/colors.
    constexpr Glyph() = default;

    /// Construct a Glyph with the provided wchar_t and Brush.
    constexpr Glyph(wchar_t sym, Brush const& b) : symbol{sym}, brush{b} {}

    /// Construct with the provided wchar_t and list of Traits and Colors.
    template <typename... Traits>
    constexpr Glyph(wchar_t sym, Traits&&... traits)
        : symbol{sym}, brush{std::forward<Traits>(traits)...}
    {}
};

// Trait -------------------------------------------------------------------
constexpr auto operator|(Glyph& g, Trait t) -> Glyph&
{
    g.brush.add_traits(t);
    return g;
}

constexpr auto operator|(Glyph&& g, Trait t) -> Glyph { return g | t; }

constexpr auto operator|(wchar_t g, Trait t) -> Glyph { return Glyph{g} | t; }

// Background_color ------------------------------------------------------------
constexpr auto operator|(Glyph& g, Background_color c) -> Glyph&
{
    g.brush.add_traits(c);
    return g;
}

constexpr auto operator|(Glyph&& g, Background_color c) -> Glyph
{
    return g | c;
}

constexpr auto operator|(wchar_t g, Background_color c) -> Glyph
{
    return Glyph{g} | c;
}

// Foreground_color ------------------------------------------------------------
constexpr auto operator|(Glyph& g, Foreground_color c) -> Glyph&
{
    g.brush.add_traits(c);
    return g;
}

constexpr auto operator|(Glyph&& g, Foreground_color c) -> Glyph
{
    return g | c;
}

constexpr auto operator|(wchar_t g, Foreground_color c) -> Glyph
{
    return Glyph{g} | c;
}

/// Compares symbol and brush for equality.
constexpr auto operator==(Glyph const& lhs, Glyph const& rhs) -> bool
{
    return (lhs.symbol == rhs.symbol) and (lhs.brush == rhs.brush);
}

/// Compares symbol and brush for inequality.
constexpr auto operator!=(Glyph const& lhs, Glyph const& rhs) -> bool
{
    return !(lhs == rhs);
}

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_GLYPH_HPP
