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
    Brush brush = Brush{};

   public:
    /// Construct an invisible Glyph, defaults to space and no traits/colors.
    constexpr Glyph() = default;

    /// Construct a Glyph with the provided char32_t and Brush.
    constexpr Glyph(char32_t sym, Brush b) : symbol{sym}, brush{b} {}

    /// Construct with the provided char32_t and list of Traits and Colors.
    template <typename... Items>
    constexpr Glyph(char32_t sym, Items... items) : symbol{sym}, brush{items...}
    {}

    /// Construct a Glyph with the provided char and Brush.
    constexpr Glyph(char sym, Brush b)
        : symbol{static_cast<char32_t>(sym)}, brush{b}
    {}

    /// Construct with the provided char32_t and list of Traits and Colors.
    template <typename... Items>
    constexpr Glyph(char sym, Items... items)
        : symbol{static_cast<char32_t>(sym)}, brush{items...}
    {}
};

// Trait -------------------------------------------------------------------
// TODO remove
auto constexpr operator|(Glyph& g, Traits ts) -> Glyph&
{
    g.brush.traits |= ts;
    return g;
}

auto constexpr operator|=(Glyph& g, Traits ts) -> Glyph&
{
    g.brush.traits |= ts;
    return g;
}

[[nodiscard]] auto constexpr operator|(Glyph const& g, Traits ts) -> Glyph
{
    auto copy = g;
    copy.brush.traits |= ts;
    return copy;
}

[[nodiscard]] auto constexpr operator|(Glyph&& g, Traits ts) -> Glyph
{
    g.brush.traits |= ts;
    return std::move(g);
}

}  // namespace ox

namespace esc {  // For ADL; Trait(s) is really in namespace::esc.

[[nodiscard]] auto constexpr operator|(char32_t g, Trait t) -> ox::Glyph
{
    return ox::Glyph{g} | t;
}

[[nodiscard]] auto constexpr operator|(char32_t g, Traits ts) -> ox::Glyph
{
    return ox::Glyph{g} | ts;
}

[[nodiscard]] auto constexpr operator|(char g, Trait t) -> ox::Glyph
{
    return ox::Glyph{g} | t;
}

[[nodiscard]] auto constexpr operator|(char g, Traits ts) -> ox::Glyph
{
    return ox::Glyph{g} | ts;
}

}  // namespace esc

namespace ox {

// Background_color ------------------------------------------------------------
// TODO remove
auto constexpr operator|(Glyph& g, Background_color c) -> Glyph&
{
    g.brush.background = Color{c.value};
    return g;
}

auto constexpr operator|=(Glyph& g, Background_color c) -> Glyph&
{
    g.brush.background = Color{c.value};
    return g;
}

// TODO don't take const&, just take by value, Glyph == 8 bytes
[[nodiscard]] auto constexpr operator|(Glyph const& g, Background_color c)
    -> Glyph
{
    auto copy = g;
    return copy | c;
}

// TODO
// Probably don't need this overload since Glyph does not allocate.
[[nodiscard]] auto constexpr operator|(Glyph&& g, Background_color c) -> Glyph
{
    g.brush.background = Color{c.value};
    return std::move(g);
}

[[nodiscard]] auto constexpr operator|(char32_t g, Background_color c) -> Glyph
{
    return Glyph{g} | c;
}

[[nodiscard]] auto constexpr operator|(char g, Background_color c) -> ox::Glyph
{
    return ox::Glyph{g} | c;
}

// Foreground_color ------------------------------------------------------------
// TODO remove
auto constexpr operator|(Glyph& g, Foreground_color c) -> Glyph&
{
    g.brush.foreground = Color{c.value};
    return g;
}

auto constexpr operator|=(Glyph& g, Foreground_color c) -> Glyph&
{
    g.brush.foreground = Color{c.value};
    return g;
}

[[nodiscard]] auto constexpr operator|(Glyph const& g, Foreground_color c)
    -> Glyph
{
    auto copy             = g;
    copy.brush.foreground = Color{c.value};
    return copy;
}

[[nodiscard]] auto constexpr operator|(Glyph&& g, Foreground_color c) -> Glyph
{
    g.brush.foreground = Color{c.value};
    return std::move(g);
}

[[nodiscard]] auto constexpr operator|(char32_t g, Foreground_color c) -> Glyph
{
    return Glyph{g} | c;
}

[[nodiscard]] auto constexpr operator|(char g, Foreground_color c) -> ox::Glyph
{
    return ox::Glyph{g} | c;
}

// Comparisons  ----------------------------------------------------------------

/// Compares symbol and brush for equality.
[[nodiscard]] auto constexpr operator==(Glyph lhs, Glyph rhs) -> bool
{
    return (lhs.symbol == rhs.symbol) && (lhs.brush == rhs.brush);
}

/// Compares symbol and brush for inequality.
[[nodiscard]] auto constexpr operator!=(Glyph lhs, Glyph rhs) -> bool
{
    return !(lhs == rhs);
}

}  // namespace ox
#endif  // TERMOX_PAINTER_GLYPH_HPP
