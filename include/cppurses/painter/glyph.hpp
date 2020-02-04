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
    Glyph(wchar_t sym, Brush const& b) : symbol{sym}, brush{b} {}

    /// Construct with the provided wchar_t and list of Attributes and Colors.
    template <typename... Attributes>
    Glyph(wchar_t sym, Attributes&&... attrs)
        : symbol{sym}, brush{std::forward<Attributes>(attrs)...}
    {}

    /// The Glyph's symbol is the wide character that will be displayed.
    wchar_t symbol = L' ';

    /// The Brush that will determine the Attributes and Colors of the symbol.
    Brush brush;
};

inline auto operator""_g(wchar_t c) -> Glyph { return {c}; }

inline auto operator|(Glyph&& g, Attribute attr) -> Glyph
{
    g.brush.add_attributes(attr);
    return std::move(g);
}

inline auto operator|(Glyph const& g, Attribute attr) -> Glyph
{
    auto result = g;
    result.brush.add_attributes(attr);
    return result;
}

inline auto operator|=(Glyph& g, Attribute attr) -> Glyph&
{
    g.brush.add_attributes(attr);
    return g;
}

inline auto operator|(Glyph&& g, detail::BackgroundColor c) -> Glyph
{
    g.brush.add_attributes(c);
    return std::move(g);
}

inline auto operator|(Glyph const& g, detail::BackgroundColor c) -> Glyph
{
    auto result = g;
    result.brush.add_attributes(c);
    return result;
}

inline auto operator|=(Glyph& g, detail::BackgroundColor c) -> Glyph&
{
    g.brush.add_attributes(c);
    return g;
}

inline auto operator|(Glyph&& g, detail::ForegroundColor c) -> Glyph
{
    g.brush.add_attributes(c);
    return std::move(g);
}

inline auto operator|(Glyph const& g, detail::ForegroundColor c) -> Glyph
{
    auto result = g;
    result.brush.add_attributes(c);
    return result;
}

inline auto operator|=(Glyph& g, detail::ForegroundColor c) -> Glyph&
{
    g.brush.add_attributes(c);
    return g;
}

/// Compares if each symbol and brush are equal.
inline bool operator==(Glyph const& lhs, Glyph const& rhs)
{
    return (lhs.symbol == rhs.symbol) and (lhs.brush == rhs.brush);
}

/// Compares if each symbol and brush are not equal.
inline bool operator!=(Glyph const& lhs, Glyph const& rhs)
{
    return !(lhs == rhs);
}

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_GLYPH_HPP
