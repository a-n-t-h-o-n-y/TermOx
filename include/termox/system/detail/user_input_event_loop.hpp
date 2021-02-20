#ifndef TERMOX_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
#define TERMOX_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
#include <termox/system/event_loop.hpp>

namespace ox::detail {

/// Event loop that blocks for user input on each iteration.
class User_input_event_loop {
   public:
    /// Starts listening for user input events in the thread called from.
    auto run() -> int;

   private:
    Event_loop loop_;
};

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
