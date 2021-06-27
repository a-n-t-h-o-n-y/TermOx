#ifndef TERMOX_COMMON_FPS_HPP
#define TERMOX_COMMON_FPS_HPP
namespace ox {

/// Frames Per Second
struct FPS {
    unsigned int value;
};

/// Converts frames per second to an interval of time.
/** Interval should be a std::chrono::duration type. */
template <typename Interval>
[[nodiscard]] auto fps_to_period(FPS fps) -> Interval
{
    return Interval{static_cast<typename Interval::rep>((1. / fps.value) *
                                                        Interval::period::den)};
}

}  // namespace ox
#endif  // TERMOX_COMMON_FPS_HPP
