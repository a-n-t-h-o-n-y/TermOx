/** @file
 * Viewport offsets, clamping, and scrollable-widget helpers.
 */
#pragma once

#include <utility>

namespace ox {

/** All lengths are nonnegative. Positions/deltas may be any int. */
[[nodiscard]] auto scroll_limit(int content, int viewport) -> int;
/** Clamp an offset to [0, max(content - viewport, 0)]; negative lengths throw. */
[[nodiscard]] auto clamp_scroll(int position, int content, int viewport) -> int;
/** Add a signed delta with overflow-safe clamping to valid scroll limits. */
[[nodiscard]] auto scroll_by(int position, int delta, int content, int viewport) -> int;
/** Reveal a range. Oversized ranges align their beginning; empty ranges do nothing. */
[[nodiscard]] auto scroll_to_show(int position,
                                  int first,
                                  int length,
                                  int content,
                                  int viewport) -> int;
/** Thumb {offset, length} and inverse mapping of a click to a centered thumb. */
[[nodiscard]] auto scrollbar_thumb(int position, int content, int viewport, int track)
    -> std::pair<int, int>;
/** Map a track cell to the scroll offset that centers the thumb there; clamp
 * out-of-track positions.
 */
[[nodiscard]] auto scrollbar_position(int cell, int content, int viewport, int track)
    -> int;

}  // namespace ox
