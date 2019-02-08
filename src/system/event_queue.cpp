#include <cppurses/system/detail/event_queue.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>

#include <cppurses/system/detail/is_sendable.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/children_data.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;

template <typename Queue_t>
void remove_descendant_events(const Widget& receiver, Queue_t& queue) {
    auto is_descendant = [&receiver](const std::unique_ptr<Event>& on_queue) {
        return receiver.children.has_descendant(&(on_queue->receiver()));
    };
    auto at = std::remove_if(std::begin(queue), std::end(queue), is_descendant);
    queue.erase(at, std::end(queue));
}

auto is(const Widget& receiver, Event::Type event_type) {
    return [event_type, &receiver](const std::unique_ptr<Event>& on_queue) {
        return &receiver == &(on_queue->receiver()) &&
               on_queue->type() == event_type;
    };
}

// Return true if an event was removed from the queue.
template <typename Queue_t>
bool remove(const Widget& receiver, Event::Type type, Queue_t& queue) {
    const auto begin = std::begin(queue);
    const auto end = std::end(queue);
    const auto erase_iter = std::remove_if(begin, end, is(receiver, type));
    queue.erase(erase_iter, end);
    return erase_iter != end;
}

bool is_expensive(Event::Type type) {
    return type == Event::Paint || type == Event::Move ||
           type == Event::Resize || type == Event::Disable ||
           type == Event::Enable;
}

}  // namespace

namespace cppurses {
namespace detail {

void Event_queue::append(std::unique_ptr<Event> event) {
    if (event == nullptr || !is_sendable(*event)) {
        return;
    }
    // Remove canceling out Enable/Disable pairs.
    auto type = event->type();
    if (type == Event::Enable) {
        bool event_removed = remove(event->receiver(), Event::Disable, queue_);
        if (event_removed) {
            return;
        }
    } else if (type == Event::Disable) {
        bool event_removed = remove(event->receiver(), Event::Enable, queue_);
        if (event_removed) {
            return;
        }
    }
    if (is_expensive(type)) {
        remove(event->receiver(), type, queue_);
    }
    if (type == Event::Delete) {
        remove_descendant_events(event->receiver(), queue_);
    }
    queue_.emplace_back(std::move(event));
}

}  // namespace detail
}  // namespace cppurses
