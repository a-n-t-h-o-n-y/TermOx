#ifndef TERMOX_PAINTER_GLYPH_HPP
#define TERMOX_PAINTER_GLYPH_HPP
#include <termox/painter/brush.hpp>

namespace ox {

/**
 * A single displayable tile.
 *
 * Holds a symbol and a Brush that defines the symbol's appearance.
 * sizeof(Glyph) == 8 Bytes && alignof(Glyph) == 4 Bytes.
 */
struct Glyph {
   public:
    char32_t symbol;
    Brush brush;

   public:
    /** This is needed for implicit conversion from `char32_t` to `Glyph`. */
    constexpr Glyph(char32_t s = U'\0', Brush b = Brush{}) : symbol{s}, brush{b}
    {}

    /** This is needed for implicit conversion from `char` to `Glyph`. */
    constexpr Glyph(char s, Brush b = Brush{})
        : symbol{static_cast<char32_t>(s)}, brush{b}
    {}
};

/** Glyphs are equality comparable. */
[[nodiscard]] auto constexpr operator==(Glyph a, Glyph b) -> bool
{
    return (a.symbol == b.symbol) && (a.brush == b.brush);
}

/** Glyphs are inequality comparable. */
[[nodiscard]] auto constexpr operator!=(Glyph a, Glyph b) -> bool
{
    return !(a == b);
}

/** Return a new `Glyph` with `bg` merged into the `Glyph`'s `Brush`. */
[[nodiscard]] auto constexpr operator|(Glyph g, Background_color bg) -> Glyph
{
    return Glyph{g.symbol, g.brush | bg};
}

/** Return a new `Glyph` with symbol `g` and background color `bg`. */
[[nodiscard]] auto constexpr operator|(char32_t g, Background_color bg) -> Glyph
{
    return Glyph{g, Brush{} | bg};
}

/** Return a new `Glyph` with symbol `g` and background color `bg`. */
[[nodiscard]] auto constexpr operator|(char g, Background_color bg) -> ox::Glyph
{
    return Glyph{static_cast<char32_t>(g), Brush{} | bg};
}

/** Return a new `Glyph` with `fg` merged into the `Glyph`'s `Brush`. */
[[nodiscard]] auto constexpr operator|(Glyph g, Foreground_color fg) -> Glyph
{
    return Glyph{g.symbol, g.brush | fg};
}

/** Return a new `Glyph` with symbol `g` and foreground color `fg`. */
[[nodiscard]] auto constexpr operator|(char32_t g, Foreground_color fg) -> Glyph
{
    return Glyph{g, Brush{} | fg};
}

/** Return a new `Glyph` with symbol `g` and foreground color `fg`. */
[[nodiscard]] auto constexpr operator|(char g, Foreground_color fg) -> ox::Glyph
{
    return Glyph{static_cast<char32_t>(g), Brush{} | fg};
}

/** Return a new `Glyph` with `Traits` merged into the `Glyph`'s `Brush`. */
[[nodiscard]] auto constexpr operator|(Glyph g, Traits ts) -> Glyph
{
    return Glyph{g.symbol, g.brush | ts};
}

}  // namespace ox

namespace esc {  // For ADL; Trait(s) is really in namespace::esc.
// Both Trait and Traits overloads are needed here.

/** Return a new `Glyph` with `Trait` merged into the `Glyph`'s `Brush`. */
[[nodiscard]] auto constexpr operator|(char32_t g, Trait t) -> ox::Glyph
{
    return ox::Glyph{g} | t;
}

/** Return a new `Glyph` with `Traits` merged into the `Glyph`'s `Brush`. */
[[nodiscard]] auto constexpr operator|(char32_t g, Traits ts) -> ox::Glyph
{
    return ox::Glyph{g} | ts;
}

/** Return a new `Glyph` with `Trait` merged into the `Glyph`'s `Brush`. */
[[nodiscard]] auto constexpr operator|(char g, Trait t) -> ox::Glyph
{
    return ox::Glyph{g} | t;
}

/** Return a new `Glyph` with `Traits` merged into the `Glyph`'s `Brush`. */
[[nodiscard]] auto constexpr operator|(char g, Traits ts) -> ox::Glyph
{
    return ox::Glyph{g} | ts;
}

}  // namespace esc
#endif  // TERMOX_PAINTER_GLYPH_HPP
