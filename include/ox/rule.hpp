#pragma once
#include <ox/core/canvas.hpp>
namespace ox {
/** Draw a capped light rule with a heavy interval, positioned in half-cell steps.
 * Coordinates are cell units along the first row (or first column when vertical).
 * Caps grow to half a cell without making short intervals disappear. A zero length
 * draws only the track. Drawing is clipped; other rows/columns are untouched. */
void paint_rule(Canvas,
                double start,
                double length,
                Style active,
                Style track,
                bool vertical = false,
                bool ascii = false);
}  // namespace ox
