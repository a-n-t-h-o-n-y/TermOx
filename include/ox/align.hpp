#pragma once

#include <cstdint>

namespace ox {

enum class Align : std::uint8_t { Left, Center, Right };

enum class VAlign : std::uint8_t { Top, Middle, Bottom };

/**
 * Return the horizontal offset from the left side of the area to align the given width.
 *
 * @param align The alignment to use.
 * @param cells_available The total number of cells available to put Glyphs to.
 * @param cells_used The number of Glyphs to put in the available cells.
 */
[[nodiscard]]
auto find_align_offset(Align align, int cells_available, int cells_used) -> int;

}  // namespace ox