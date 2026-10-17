/** @file
 * Text and child alignment modes.
 */
#pragma once

#include <cstdint>

namespace ox {

/** Alignment along the text direction. Left/Right are horizontal spellings. */
enum class Align : std::uint8_t {
    Start, ///< Place content at the beginning of the axis.
    Center, ///< Center content along the axis.
    End, ///< Place content at the end of the axis.
    Left = Start, ///< Horizontal spelling of Start.
    Right = End ///< Horizontal spelling of End.
};

/**
 * Return the offset along an axis in cells. Nonnegative extents are required;
 * oversized content can produce a negative offset.
 *
 * @param align The alignment to use.
 * @param cells_available The total number of cells available to put Glyphs to.
 * @param cells_used The number of terminal columns used by the content.
 */
[[nodiscard]]
auto align_offset(Align align, int cells_available, int cells_used) -> int;

}  // namespace ox