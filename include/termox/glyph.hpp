#pragma once

#include <esc/brush.hpp>
#include <esc/color.hpp>

namespace ox {

using ::esc::Brush;
using ::esc::Color;
using ::esc::ColorIndex;
using ::esc::DefaultColor;
using ::esc::TrueColor;

/**
 * @brief A single terminal cell display unit.
 *
 * Contains a symbol and a Brush to describe how the symbol should be displayed.
 */
struct Glyph {
    char32_t symbol  = U'\0';
    esc::Brush brush = {};
};

/**
 * @brief Compares the symbol and brush of two Glyphs for equality
 *
 * @param lhs The left hand side of the comparison.
 * @param rhs The right hand side of the comparison.
j* @return true if the symbol and brush are equal, false otherwise.
*/
[[nodiscard]] inline auto operator==(Glyph const& lhs, Glyph const& rhs) -> bool
{
    return lhs.symbol == rhs.symbol && lhs.brush == rhs.brush;
}

/**
 * @brief Compares the symbol and brush of two Glyphs for inequality
 *
 * @param lhs The left hand side of the comparison.
 * @param rhs The right hand side of the comparison.
 * @return true if the symbol and brush are not equal, false otherwise.
 */
[[nodiscard]] inline auto operator!=(Glyph const& lhs, Glyph const& rhs) -> bool
{
    return !(lhs == rhs);
}

}  // namespace ox