#pragma once

#include <esc/brush.hpp>

namespace ox {

/**
 * @brief A single terminal cell display unit.
 *
 * Contains a symbol and a Brush to describe how the symbol should be displayed.
 */
struct Glyph {
    char32_t symbol;
    esc::Brush brush;
};

/**
 * @brief Compares the symbol and brush of two Glyphs.
 *
 * @param lhs The left hand side of the comparison.
 * @param rhs The right hand side of the comparison.
j* @return true if the symbol and brush are equal, false otherwise.
*/
[[nodiscard]] inline auto operator==(Glyph const& lhs, Glyph const& rhs) -> bool
{
    return lhs.symbol == rhs.symbol && lhs.brush == rhs.brush;
}

}  // namespace ox