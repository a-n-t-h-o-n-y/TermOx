#include <ox/scroll.hpp>

#include <algorithm>
#include <cstdint>
#include <stdexcept>

namespace ox {

auto scroll_limit(int content, int viewport) -> int
{
    if (content < 0 || viewport < 0) {
        throw std::invalid_argument{"Scroll lengths must be nonnegative"};
    }
    return std::max(content - viewport, 0);
}
auto clamp_scroll(int position, int content, int viewport) -> int
{ return std::clamp(position, 0, scroll_limit(content, viewport)); }
auto scroll_by(int position, int delta, int content, int viewport) -> int
{
    return (int)std::clamp(
        std::int64_t{clamp_scroll(position, content, viewport)} + delta,
        std::int64_t{0}, (std::int64_t)scroll_limit(content, viewport));
}
auto scroll_to_show(int position, int first, int length, int content, int viewport)
    -> int
{
    if (first < 0 || length < 0) {
        throw std::invalid_argument{"Scroll range must be nonnegative"};
    }
    position = clamp_scroll(position, content, viewport);
    if (length == 0 || viewport == 0) { return position; }
    if (first < position || length > viewport) {
        return clamp_scroll(first, content, viewport);
    }
    auto const end = std::int64_t{first} + length;
    if (end > std::int64_t{position} + viewport) {
        return (int)std::min(end - viewport,
                             (std::int64_t)scroll_limit(content, viewport));
    }
    return position;
}
auto scrollbar_thumb(int position, int content, int viewport, int track)
    -> std::pair<int, int>
{
    if (track < 0) { throw std::invalid_argument{"Scroll track must be nonnegative"}; }
    auto const limit = scroll_limit(content, viewport);
    if (track == 0) { return {0, 0}; }
    if (limit == 0) { return {0, track}; }
    auto const length =
        (int)std::clamp((std::int64_t{track} * viewport + content - 1) / content,
                        std::int64_t{1}, (std::int64_t)std::max(track - 1, 1));
    auto const travel = track - length;
    auto const offset =
        (int)((std::int64_t{clamp_scroll(position, content, viewport)} * travel +
               limit / 2) /
              limit);
    return {offset, length};
}
auto scrollbar_position(int cell, int content, int viewport, int track) -> int
{
    auto const [offset, length] = scrollbar_thumb(0, content, viewport, track);
    (void)offset;
    auto const travel = track - length;
    if (travel == 0) { return 0; }
    auto const top = std::clamp(std::int64_t{cell} - length / 2, std::int64_t{0},
                                (std::int64_t)travel);
    return (int)((top * scroll_limit(content, viewport) + travel / 2) / travel);
}
}  // namespace ox
