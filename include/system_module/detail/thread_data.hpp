#ifndef SYSTEM_MODULE_DETAIL_THREAD_DATA_HPP
#define SYSTEM_MODULE_DETAIL_THREAD_DATA_HPP

#include "system_module/abstract_event_dispatcher.hpp"
#include "system_module/event_loop.hpp"
#include "system_module/detail/ncurses_event_dispatcher.hpp"
#include "system_module/detail/posted_event_queue.hpp"
#include <memory>
#include <vector>

namespace cppurses {
namespace detail {

class Thread_data {
   public:
    static Thread_data& current();
    Abstract_event_dispatcher& dispatcher();

    bool can_wait = true;
    bool quit_now = false;
    int loop_level = 0;
    std::vector<Event_loop*> event_loops;
    Posted_event_queue event_queue;

   private:
    std::unique_ptr<Abstract_event_dispatcher> dispatcher_ =
        std::make_unique<NCurses_event_dispatcher>();
    Thread_data() = default;
};

}  // namespace detail
}  // namespace cppurses
#endif  // SYSTEM_MODULE_DETAIL_THREAD_DATA_HPP
