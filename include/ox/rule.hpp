#pragma once
#include <ox/core/canvas.hpp>
namespace ox {
/** Draw a capped rule with a colored interval rounded to whole cells.
 * Coordinates run along the first row, or first column when vertical.
 * A zero length draws only the track. Other rows/columns are untouched. */
void paint_rule(Canvas,
                double start,
                double length,
                Style active,
                Style track,
                bool vertical = false,
                bool ascii = false);
}  // namespace ox
