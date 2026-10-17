/** @file
 * Cell coordinates, extents, rectangles, and clipping arithmetic.
 * `Point` and `Area` are re-exported from Escape. Point has signed integer x/y
 * members; Area has signed integer width/height members. TermOx validates extents
 * at API boundaries, while origins may be negative for clipping and scrolling.
 */
#pragma once

#include <optional>

#include <esc/area.hpp>
#include <esc/point.hpp>

namespace ox {
using ::esc::Area;
using ::esc::Point;
/** Optional terminal-cell position; nullopt means no visible caret. */
using Cursor = std::optional<Point>;

/** A half-open rectangle in terminal columns and rows. */
struct Rect {
    /** Top-left origin; negative origins are allowed for clipped views. */
    Point at = {0, 0};
    /** Nonnegative width and height; a zero extent makes the rectangle empty. */
    Area size = {0, 0};
    /** Compare origin and extent exactly. */
    auto operator==(Rect const&) const -> bool = default;
};

/** Reject negative dimensions; empty rectangles are valid. */
void validate(Area size);
/** Return the half-open overlap, with empty extents when rectangles do not overlap;
 * coordinates use checked arithmetic.
 */
[[nodiscard]] auto intersection(Rect one, Rect two) -> Rect;
/** Return whether point lies inside the half-open rectangle; empty rectangles contain
 * nothing.
 */
[[nodiscard]] auto contains(Rect rectangle, Point point) -> bool;
/** Add coordinates with checked arithmetic. */
[[nodiscard]] auto translate(Point point, Point offset) -> Point;
}  // namespace ox
