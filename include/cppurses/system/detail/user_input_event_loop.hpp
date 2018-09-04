#ifndef CPPURSES_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
#define CPPURSES_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
#include <cppurses/system/event_loop.hpp>

#include <cppurses/system/detail/ncurses_event_listener.hpp>

namespace cppurses {
namespace detail {

/// Event loop that blocks for user input on each iteration.
/** Uses ncurses internally to get input. */
class User_input_event_loop : public Event_loop {
   protected:
    /// Waits on NCurses_event_listener::get_input(), and posts the result.
    void loop_function() override;

   private:
    NCurses_event_listener event_listener_;
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
