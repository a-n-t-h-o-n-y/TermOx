#pragma once

#include <optional>

#include <esc/area.hpp>
#include <esc/point.hpp>

namespace ox {
using ::esc::Area;
using ::esc::Point;
using Cursor = std::optional<Point>;

/** A half-open rectangle in terminal columns and rows. */
struct Rect {
    Point at = {0, 0};
    Area size = {0, 0};
    auto operator==(Rect const&) const -> bool = default;
};

/** Reject negative dimensions; empty rectangles are valid. */
void validate(Area size);
[[nodiscard]] auto intersection(Rect one, Rect two) -> Rect;
[[nodiscard]] auto contains(Rect rectangle, Point point) -> bool;
/** Add coordinates with checked arithmetic. */
[[nodiscard]] auto translate(Point point, Point offset) -> Point;
}  // namespace ox
