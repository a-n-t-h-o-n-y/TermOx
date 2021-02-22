#ifndef TERMOX_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
#define TERMOX_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
#include <termox/system/event_loop.hpp>
#include <termox/system/event_queue.hpp>

namespace ox::detail {

/// Event loop that blocks for user input on each iteration.
class User_input_event_loop {
   public:
    /// Starts listening for user input events in the thread called from.
    auto run() -> int;

    /// Sets exit flag.
    auto exit(int exit_code) { loop_.exit(exit_code); }

    /// Return reference to the internal Event_queue.
    /** Used by System to initialize the current queue. */
    auto event_queue() -> Event_queue& { return loop_.event_queue(); }

   private:
    Event_loop loop_;
};

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_USER_INPUT_EVENT_LOOP_HPP
