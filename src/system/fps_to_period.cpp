#include <cppurses/system/detail/fps_to_period.hpp>

#include <cppurses/system/detail/timer_event_loop.hpp>

namespace cppurses {
namespace detail {

typename Timer_event_loop::Period_t fps_to_period(int fps) {
    return typename Timer_event_loop::Period_t(
        static_cast<typename Timer_event_loop::Period_t::rep>(
            (1.0 / static_cast<double>(fps)) *
            Timer_event_loop::Period_t::period::den));
}

}  // namespace detail
}  // namespace cppurses
