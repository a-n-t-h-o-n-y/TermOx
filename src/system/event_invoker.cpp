#include "system/detail/event_invoker.hpp"
#include "system/event.hpp"
#include "system/system.hpp"
#include "system/event_handler.hpp"

namespace {
using namespace cppurses;

void send_event(const Event& event) {
    bool handled = event.send_to_all_filters();
    if (!handled) {
        event.send();
    }
}

}  // namespace

namespace cppurses {
namespace detail {

void Event_invoker::invoke(Event_queue& queue,
                           Event::Type type_filter,
                           Event_handler* object_filter) {
    auto event_iter = std::begin(queue.queue_);
    while (event_iter != std::end(queue.queue_)) {
        Event_handler* receiver = (*event_iter)->receiver();
        auto type = (*event_iter)->type();
        // Object Filter
        if (receiver != object_filter && object_filter != nullptr) {
            ++event_iter;
            continue;
        }
        // Deferred Delete Filter
        if (Event::DeferredDelete == type &&
            Event::DeferredDelete != type_filter) {
            ++event_iter;
            continue;
        }
        // Event Filter Match OR No Event Filter - Send Event
        if (type_filter == Event::None || type_filter == type) {
            auto event = std::move(*event_iter);
            queue.queue_.erase(event_iter);
            send_event(*event);
            event_iter = std::begin(queue.queue_);
            continue;
        }
    }
}

}  // namespace detail
}  // namespace cppurses
