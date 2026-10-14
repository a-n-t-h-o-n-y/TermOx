#pragma once

#include <utility>

namespace ox {

/** All lengths are nonnegative. Positions/deltas may be any int. */
[[nodiscard]] auto scroll_limit(int content, int viewport) -> int;
[[nodiscard]] auto clamp_scroll(int position, int content, int viewport) -> int;
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
[[nodiscard]] auto scrollbar_position(int cell, int content, int viewport, int track)
    -> int;

}  // namespace ox
