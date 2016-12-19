#ifndef THREAD_DATA_HPP
#define THREAD_DATA_HPP

#include "../abstract_event_dispatcher.hpp"
#include "../event_loop.hpp"
#include "ncurses_event_dispatcher.hpp"
#include "posted_event_queue.hpp"

#include <memory>
#include <stack>

namespace mcurses {
namespace detail {

class Thread_data {
   public:
    static Thread_data& current();
    Abstract_event_dispatcher& dispatcher();

    bool can_wait = true;  // is true if no more events to process on queue
    bool quit_now = false;
    int loop_level = 0;
    std::stack<Event_loop*> event_loops;
    Posted_event_queue event_queue;

   private:
    std::unique_ptr<Abstract_event_dispatcher> dispatcher_ =
        std::make_unique<NCurses_event_dispatcher>();
    Thread_data() = default;
    static Thread_data data_;
};

}  // namespace detail
}  // namespace mcurses
#endif  // THREAD_DATA_HPP
