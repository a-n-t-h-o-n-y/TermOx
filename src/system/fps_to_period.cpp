#include <cppurses/system/detail/fps_to_period.hpp>

#include <cppurses/system/detail/animation_event_loop.hpp>

namespace cppurses {
namespace detail {

typename Animation_event_loop::Period_t fps_to_period(int fps) {
    return typename Animation_event_loop::Period_t(
        static_cast<typename Animation_event_loop::Period_t::rep>(
            (1.0 / static_cast<double>(fps)) *
            Animation_event_loop::Period_t::period::den));
}

}  // namespace detail
}  // namespace cppurses
