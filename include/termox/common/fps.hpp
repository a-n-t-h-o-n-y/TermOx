#ifndef TERMOX_COMMON_FPS_HPP
#define TERMOX_COMMON_FPS_HPP
#include <cstdint>

namespace ox {

/// Frames Per Second
struct FPS {
    std::uint16_t value;
};

/// Converts frames per second to an interval of time.
/** Duration_t should be/mimic a std::chrono::duration type. */
template <typename Duration_t>
[[nodiscard]] auto fps_to_period(FPS fps) -> Duration_t
{
    return Duration_t{static_cast<typename Duration_t::rep>(
        (1. / fps.value) * Duration_t::period::den)};
}

}  // namespace ox
#endif  // TERMOX_COMMON_FPS_HPP
