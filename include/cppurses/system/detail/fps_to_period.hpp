#ifndef CPPURSES_SYSTEM_DETAIL_FPS_TO_PERIOD_HPP
#define CPPURSES_SYSTEM_DETAIL_FPS_TO_PERIOD_HPP
#include <cppurses/system/detail/animation_event_loop.hpp>

namespace cppurses {
namespace detail {

/// Converts frames per second \p fps to a period.
typename detail::Animation_event_loop::Period_t fps_to_period(int fps);

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_FPS_TO_PERIOD_HPP
