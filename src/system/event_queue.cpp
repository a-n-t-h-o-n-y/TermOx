#include "system/detail/event_queue.hpp"
#include "system/event.hpp"
#include <algorithm>
#include <memory>

namespace cppurses {
namespace detail {

void Event_queue::append(std::unique_ptr<Event> event) {
    if (event == nullptr) {
        return;
    }
    // Optimize out duplicate expensive events.
    Event::Type type = event->type();
    if (type == Event::Paint || type == Event::Move || type == Event::Resize ||
        type == Event::ClearScreen) {
        auto is_same_event = [&type, &event](const auto& e) {
            return (e->type() == type) && (e->receiver() == event->receiver());
        };
        auto begin = std::begin(queue_);
        auto end = std::end(queue_);
        auto position = std::find_if(begin, end, is_same_event);
        if (position != end) {
            queue_.erase(position);
        }
    }
    queue_.emplace_back(std::move(event));
}

}  // namespace detail
}  // namespace cppurses
