#pragma once

#include <esc/brush.hpp>
#include <esc/color.hpp>
#include <esc/trait.hpp>

namespace ox {

using ::esc::Brush;
using ::esc::Color;
using ::esc::ColorIndex;
using ::esc::DefaultColor;
using ::esc::Trait;
using ::esc::Traits;
using ::esc::TrueColor;

/**
 * @brief A single terminal cell display unit.
 *
 * Contains a symbol and a Brush to describe how the symbol should be displayed.
 */
struct Glyph {
    char32_t symbol = U'\0';
    Brush brush     = {};

    [[nodiscard]] constexpr bool operator==(Glyph const&) const = default;
    [[nodiscard]] constexpr bool operator!=(Glyph const&) const = default;
};

// PIPE OPS --------------------------------------------------------------------

/**
 * @brief Creates a Glyph with the given symbol and background Color.
 *
 * @param symbol The symbol to display.
 * @param c The ColorBG to apply to the return Glyph's brush.
 * @return A Glyph with the given symbol and Color background.
 */
[[nodiscard]] constexpr auto operator|(char32_t symbol, esc::ColorBG c) -> Glyph
{
    return {symbol, {.background = c.value}};
}

/**
 * @brief Copy the Glyph and modify its background Color.
 *
 * @param g The Glyph to work from.
 * @param c The ColorBG to apply to the return Glyph's brush.
 * @return The passed in Glyph with the given Color background.
 */
[[nodiscard]] constexpr auto operator|(Glyph g, esc::ColorBG c) -> Glyph
{
    g.brush.background = c.value;
    return g;
}

/**
 * @brief Creates a Glyph with the given symbol and foreground Color.
 *
 * @param symbol The symbol to display.
 * @param c The ColorFG to apply to the return Glyph's brush.
 * @return A Glyph with the given symbol and Color foreground.
 */
[[nodiscard]] constexpr auto operator|(char32_t symbol, esc::ColorFG c) -> Glyph
{
    return {symbol, {.foreground = c.value}};
}

/**
 * @brief Copy the Glyph and modify its foreground Color.
 *
 * @param g The Glyph to work from.
 * @param c The ColorFG to apply to the return Glyph's brush.
 * @return The passed in Glyph with the given Color foreground.
 */
[[nodiscard]] constexpr auto operator|(Glyph g, esc::ColorFG c) -> Glyph
{
    g.brush.foreground = c.value;
    return g;
}

/**
 * @brief Creates a Glyph with the given symbol and Trait.
 *
 * @param symbol The symbol to display.
 * @param t The Trait to apply to the return Glyph's brush.
 * @return A Glyph with the given symbol and Trait.
 */
[[nodiscard]] constexpr auto operator|(char32_t symbol, Trait t) -> Glyph
{
    return {symbol, {.traits = t}};
}

/**
 * @brief Copy the Glyph and add a Trait.
 *
 * @param g The Glyph to work from.
 * @param t The Trait to apply to the return Glyph's brush.
 * @return The passed in Glyph with the given Trait added.
 */
[[nodiscard]] constexpr auto operator|(Glyph g, Trait t) -> Glyph
{
    g.brush.traits = g.brush.traits | t;
    return g;
}

/**
 * @brief Removes a Trait from a Glyph's brush.
 *
 * Use the esc::remove_trait(Trait) function to create the RemoveTrait object.
 *
 * @param g The Glyph to remove the Trait from.
 * @param t The Trait to remove from the Glyph's brush.
 * @return The Glyph with the Trait removed from its brush.
 */
[[nodiscard]] constexpr auto operator|(Glyph g, esc::RemoveTrait t) -> Glyph
{
    g.brush.traits = g.brush.traits | t;
    return g;
}

}  // namespace ox