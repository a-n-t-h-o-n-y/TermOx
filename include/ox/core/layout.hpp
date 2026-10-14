#pragma once

#include <limits>
#include <span>
#include <vector>

#include <ox/core/geometry.hpp>

namespace ox {

/** Bounds and relative growth weight along one layout axis. */
struct SizePolicy {
    int minimum = 0;
    int maximum = std::numeric_limits<int>::max();
    float flexibility = 1.f;
    auto operator==(SizePolicy const&) const -> bool = default;

    [[nodiscard]] static constexpr auto fixed(int size) -> SizePolicy
    {
        return {
            .minimum = size,
            .maximum = size,
            .flexibility = 0.f,
        };
    }

    [[nodiscard]] static constexpr auto flex(float flex = 1.f) -> SizePolicy
    {
        return {
            .minimum = 0,
            .maximum = std::numeric_limits<int>::max(),
            .flexibility = flex,
        };
    }

    [[nodiscard]] static constexpr auto bounded(int min, int max) -> SizePolicy
    {
        return {
            .minimum = min,
            .maximum = max,
            .flexibility = 1.f,
        };
    }

    [[nodiscard]] static constexpr auto min(int min) -> SizePolicy
    {
        return {
            .minimum = min,
            .maximum = std::numeric_limits<int>::max(),
            .flexibility = 1.f,
        };
    }

    [[nodiscard]] static constexpr auto max(int max) -> SizePolicy
    {
        return {
            .minimum = 0,
            .maximum = max,
            .flexibility = 1.f,
        };
    }
};

/** Allocate integer lengths. Invalid bounds, weights, or available space throw.
 * When minimums do not fit, shrink proportionally. Otherwise grow positive weights
 * up to maximums. Round by largest remainder, breaking ties in input order.
 */
[[nodiscard]] auto distribute_length(std::span<SizePolicy const> policies,
                                     int available) -> std::vector<int>;

/** Place children consecutively from the origin; the cross axis fills the area. */
[[nodiscard]] auto layout_row(std::span<SizePolicy const> policies, Area available)
    -> std::vector<Rect>;
[[nodiscard]] auto layout_column(std::span<SizePolicy const> policies, Area available)
    -> std::vector<Rect>;

/** Clamp a requested size to its available area and center it, rounding toward
 * the top-left. Negative dimensions throw. */
[[nodiscard]] auto centered_rect(Area available, Area requested) -> Rect;

}  // namespace ox
