#pragma once

#include <cstdint>

namespace ox {

enum class Align : std::uint8_t { Left, Center, Right };

/**
 * Return the horizontal offset in terminal columns. Nonnegative extents are required;
 * oversized content can produce a negative offset.
 *
 * @param align The alignment to use.
 * @param cells_available The total number of cells available to put Glyphs to.
 * @param cells_used The number of terminal columns used by the content.
 */
[[nodiscard]]
auto align_offset(Align align, int cells_available, int cells_used) -> int;

}  // namespace ox