#ifndef CPPURSES_SYSTEM_DETAIL_FPS_TO_PERIOD_HPP
#define CPPURSES_SYSTEM_DETAIL_FPS_TO_PERIOD_HPP
#include <cppurses/system/detail/timer_event_loop.hpp>

namespace cppurses {
namespace detail {

/// Converts frames per second \p fps to a period.
/** Not currently in use, except by demo, might be worth moving to some public
 *  utility namespace. */
typename detail::Timer_event_loop::Period_t fps_to_period(int fps);

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_FPS_TO_PERIOD_HPP
