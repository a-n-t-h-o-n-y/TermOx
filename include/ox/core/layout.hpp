/** @file
 * Axis size policies, space allocation, and cross-axis placement.
 */
#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <span>
#include <vector>

#include <ox/core/geometry.hpp>

namespace ox {

/** Bounds and relative growth weight along one layout axis. */
struct SizePolicy {
    /** Smallest requested allocation in cells; nonnegative. */
    int minimum = 0;
    /** Largest allocation in cells; must be at least minimum. */
    int maximum = std::numeric_limits<int>::max();
    /** Finite nonnegative share of surplus space; zero does not grow beyond the minimum. */
    float flexibility = 1.f;
    /** Compare bounds and growth weight exactly. */
    auto operator==(SizePolicy const&) const -> bool = default;

    /** Request exactly size cells on an axis; the allocator may shrink it when all
     * minimums cannot fit.
     */
    [[nodiscard]] static constexpr auto fixed(int size) -> SizePolicy
    {
        return {
            .minimum = size,
            .maximum = size,
            .flexibility = 0.f,
        };
    }

    /** Allow unbounded growth with the supplied finite nonnegative weight. */
    [[nodiscard]] static constexpr auto flex(float flex = 1.f) -> SizePolicy
    {
        return {
            .minimum = 0,
            .maximum = std::numeric_limits<int>::max(),
            .flexibility = flex,
        };
    }

    /** Allow growth between min and max cells with unit weight; allocation validates the
     * bounds.
     */
    [[nodiscard]] static constexpr auto bounded(int min, int max) -> SizePolicy
    {
        return {
            .minimum = min,
            .maximum = max,
            .flexibility = 1.f,
        };
    }

    /** Request at least min cells with unbounded unit-weight growth. */
    [[nodiscard]] static constexpr auto min(int min) -> SizePolicy
    {
        return {
            .minimum = min,
            .maximum = std::numeric_limits<int>::max(),
            .flexibility = 1.f,
        };
    }

    /** Allow unit-weight growth from zero up to max cells. */
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

/** Child placement across the layout's main axis: fill, leading edge, center, or
 * trailing edge.
 */
enum class CrossAlign : std::uint8_t {
    Stretch, ///< Use the available cross-axis extent, constrained by the child policy.
    Start, ///< Place the child at the cross-axis beginning.
    Center, ///< Center the child on the cross axis.
    End ///< Place the child at the cross-axis end.
};
/** Clamp a natural extent against an axis policy and available space. */
[[nodiscard]] auto fit_length(int natural,
                              SizePolicy policy,
                              int available,
                              bool stretch) -> int;
/** Top/right/bottom/left padding. Negative padding is rejected. */
[[nodiscard]] auto padding_rect(Area available, std::array<int, 4> padding) -> Rect;

/** Place children consecutively from the origin; the cross axis fills the area. */
[[nodiscard]] auto layout_row(std::span<SizePolicy const> policies,
                              Area available,
                              int gap = 0) -> std::vector<Rect>;
/** Allocate vertical child extents with gaps; children fill the cross axis. Invalid
 * policies or negative gaps throw.
 */
[[nodiscard]] auto layout_column(std::span<SizePolicy const> policies,
                                 Area available,
                                 int gap = 0) -> std::vector<Rect>;

/** Clamp a requested size to its available area and center it, rounding toward
 * the top-left. Negative dimensions throw. */
[[nodiscard]] auto centered_rect(Area available, Area requested) -> Rect;

}  // namespace ox
