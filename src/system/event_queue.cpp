#include <cppurses/system/detail/event_queue.hpp>

#include <algorithm>
#include <iterator>
#include <memory>

#include <cppurses/system/event.hpp>
#include <cppurses/system/events/deferred_delete_event.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;
// TODO is the below really necessary - think about if a descendant and ancestor
// both have dd events.
// Removes deferred delete events from the queue if an ancestor of one is added.
template <typename Queue_t>
void remove_deferred_delete_events(const Event& new_event, Queue_t& queue) {
    Widget* parent =
        static_cast<const Deferred_delete_event&>(new_event).to_delete();

    auto is_descendants_event = [parent](const auto& event_ptr) {
        if (event_ptr == nullptr) {
            return false;
        }
        if (event_ptr->type() == Event::DeferredDelete) {
            Widget* child =
                static_cast<Deferred_delete_event&>(*event_ptr).to_delete();
            return parent->children.has_descendant(child);
        }
        return false;
    };
    auto pos = std::remove_if(std::begin(queue), std::end(queue),
                              is_descendants_event);
    queue.erase(pos, std::end(queue));
}
}  // namespace

namespace cppurses {
namespace detail {

void Event_queue::append(std::unique_ptr<Event> event) {
    if (event == nullptr || event->receiver() == nullptr) {
        return;
    }
    // Optimize out duplicate expensive events.
    Event::Type type = event->type();
    if (type == Event::Paint || type == Event::Repaint || type == Event::Move ||
        type == Event::Resize || type == Event::ClearScreen ||
        type == Event::DeferredDelete) {
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
            remove_deferred_delete_events(*event, queue_);
        }
    }
    queue_.emplace_back(std::move(event));
}

}  // namespace detail
}  // namespace cppurses
