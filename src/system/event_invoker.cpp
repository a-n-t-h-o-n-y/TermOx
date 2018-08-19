#include <cppurses/system/detail/event_invoker.hpp>

#include <iterator>
#include <memory>
#include <mutex>
#include <utility>

#include <cppurses/system/detail/event_queue.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/system.hpp>

#define DEBUG_EVENT_INVOKER

#if defined(DEBUG_EVENT_INVOKER)
#include <fstream>
#include <string>
#include <unordered_map>

#include <cppurses/system/detail/event_as_string.hpp>
#include <cppurses/widget/widget.hpp>
#endif

namespace cppurses {
class Event_handler;
namespace detail {

void Event_invoker::invoke(Event_queue& queue,
                           Event::Type type_filter,
                           Event_handler* object_filter) {
#if defined(DEBUG_EVENT_INVOKER)
    std::unordered_map<Event_handler*, std::vector<Event::Type>> widgs;
#endif

    auto event_iter = std::begin(queue.queue_);
    while (event_iter != std::end(queue.queue_)) {
        Event_handler* receiver = (*event_iter)->receiver();
        auto event_type = (*event_iter)->type();
        // Paint Event Filter
        if (Event::Paint == event_type && Event::Paint != type_filter) {
            ++event_iter;
            continue;
        }
        // Object Filter
        if (receiver != object_filter && object_filter != nullptr) {
            ++event_iter;
            continue;
        }
        // Delete Event Filter
        if (Event::Delete == event_type && Event::Delete != type_filter) {
            ++event_iter;
            continue;
        }
        // Event Filter Match OR No Event Filter - Send Event
        if (type_filter == Event::None || type_filter == event_type) {
#if defined(DEBUG_EVENT_INVOKER)
            widgs[receiver].push_back(event_type);
#endif
            std::unique_ptr<Event> event = std::move(*event_iter);
            queue.queue_.erase(event_iter);
            // Below call will might add to the queue_, invalidating iterators.
            System::send_event(*event);
            event_iter = std::begin(queue.queue_);
            continue;
        }
    }
#if defined(DEBUG_EVENT_INVOKER)
    std::ofstream l{"events_log.txt", std::ios::app};
    for (const auto& w_events_pair : widgs) {
        l << w_events_pair.first;
        std::string name{static_cast<Widget*>(w_events_pair.first)->name()};
        l << ' ' << name;
        l << ":\n\t";
        std::string seperator;
        for (Event::Type event_t : w_events_pair.second) {
            l << seperator << detail::event_type_as_string(event_t);
            seperator = ", ";
        }
        l << '\n';
    }
    l << "\n--->End Queue Invokation";
    l << std::endl;
#endif
}

}  // namespace detail
}  // namespace cppurses
