#include "system/detail/event_queue.hpp"
#include "system/event.hpp"
#include "system/events/deferred_delete_event.hpp"
#include "widget/widget.hpp"
#include <algorithm>
#include <memory>

namespace cppurses {
namespace detail {

void Event_queue::append(std::unique_ptr<Event> event) {
    if (event == nullptr || event->receiver() == nullptr) {
        return;
    }
    // Optimize out duplicate expensive events.
    Event::Type type = event->type();
    if (type == Event::Paint || type == Event::Move || type == Event::Resize ||
        type == Event::ClearScreen || type == Event::DeferredDelete) {
        auto is_same_event = [&type, &event](const auto& e) {
            return (e->type() == type) && (e->receiver() == event->receiver());
        };
        auto begin = std::begin(queue_);
        auto end = std::end(queue_);
        auto position = std::find_if(begin, end, is_same_event);
        if (position != end) {
            queue_.erase(position);
        }
        if (type == Event::DeferredDelete) {
            remove_dd_children(*event);
        }
    }
    queue_.emplace_back(std::move(event));
}

void Event_queue::remove_dd_children(const Event& new_event) {
    Widget* parent =
        static_cast<const Deferred_delete_event&>(new_event).to_delete();
    auto is_child = [parent](const auto& event_ptr) {
        if (event_ptr == nullptr) {
            return false;
        }
        if (event_ptr->type() == Event::DeferredDelete) {
            Widget* child =
                static_cast<Deferred_delete_event&>(*event_ptr).to_delete();
            return parent->contains_child(child);
        }
        return false;
    };
    auto pos = std::remove_if(std::begin(queue_), std::end(queue_), is_child);
    queue_.erase(pos, std::end(queue_));
}

}  // namespace detail
}  // namespace cppurses
