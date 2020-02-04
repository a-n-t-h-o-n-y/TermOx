#ifndef CPPURSES_SYSTEM_DETAIL_FPS_TO_PERIOD_HPP
#define CPPURSES_SYSTEM_DETAIL_FPS_TO_PERIOD_HPP
#include <cppurses/system/detail/timer_event_loop.hpp>

namespace cppurses {
namespace detail {

/// Converts frames per second \p fps to a period.
/** Not currently in use, except by demo, might be worth moving to some public
 *  utility namespace. */
auto fps_to_period(double fps) -> typename detail::Timer_event_loop::Period_t
{
    using Period_t = typename Timer_event_loop::Period_t;
    return Period_t(
        static_cast<Period_t::rep>((1. / fps) * Period_t::period::den));
}

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_FPS_TO_PERIOD_HPP
