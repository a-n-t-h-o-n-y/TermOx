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
    wchar_t symbol = L' ';  // 4 bytes

    /// The Brush that will determine the Traits and Colors of the symbol.
    Brush brush{};  // 5 bytes

    // sizeof(Glyph) == 12
    // alignof(Glyph) == 4
    // actual size without padding == 9
    // if Brush could be 4 bytes, then you have sizeof(Glyph) == 8
    // instead of optional colors, you can default then to the Background and
    // Foreground values, this gets rid of some checks, but it possibly disables
    // the ability to have separate default colors for separate Widgets, look
    // into it. you can actually just encode it in the enum, you've already done
    // this, with Background and Foreground, but instead of them being colors,
    // they should be indicators to use the Widget's set background and
    // foreground color or however it works. Then you have the same checks you
    // currently have on the optional, but instead it is on the Color's exact
    // value.. but black and white colors have the same value as background and
    // foreground, so you do have to change that, and if they are used in a
    // context that does not get checked, then it breaks..
    // 1; Color::Background and Color::Foreground need unique values
    // 2; the optional check, probably in painter, on the widget's brush, needs
    // to check for these values and turn them into either the Widget defaults
    // or the Terminal defaults.

   public:
    /// Construct an invisible Glyph, defaults to space and no traits/colors.
    constexpr Glyph() = default;

    /// Construct a Glyph with the provided wchar_t and Brush.
    constexpr Glyph(wchar_t sym, Brush b) : symbol{sym}, brush{b} {}

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
