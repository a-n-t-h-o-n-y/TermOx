#ifndef CPPURSES_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
#define CPPURSES_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
#include <cppurses/system/event_loop.hpp>

namespace cppurses {
namespace detail {

// TODO This could be a function that returns an Event_loop instead.

/// Event loop that blocks for user input on each iteration. Uses ncurses.
class User_input_event_loop : public Event_loop {
   public:
    User_input_event_loop();
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
