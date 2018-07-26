#ifndef CPPURSES_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
#define CPPURSES_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
#include <cppurses/system/event_loop.hpp>

namespace cppurses {
namespace detail {

class User_input_event_loop : public Event_loop {
   public:
    User_input_event_loop();
};

}  // namespace detail
}  // namespace cppurses

#endif  // CPPURSES_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
