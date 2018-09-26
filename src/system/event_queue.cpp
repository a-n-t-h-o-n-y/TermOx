#include <cppurses/system/detail/event_queue.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>

#include <cppurses/system/event.hpp>
#include <cppurses/widget/children_data.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;

template <typename Queue_t>
void remove_descendant_delete_events(const Event& new_event, Queue_t& queue) {
    const Widget* receiver{new_event.receiver()};
    auto is_descendant = [receiver](const std::unique_ptr<Event>& on_queue) {
        return receiver->children.has_descendant(on_queue->receiver());
    };
    auto at = std::remove_if(std::begin(queue), std::end(queue), is_descendant);
    queue.erase(at, std::end(queue));
}

}  // namespace

namespace cppurses {
namespace detail {

void Event_queue::append(std::unique_ptr<Event> event) {
    if (event == nullptr || event->receiver() == nullptr ||
        (!event->receiver()->enabled() &&
         (event->type() != Event::Delete && event->type() != Event::Disable))) {
        // TODO provide a check for Disable_event to the above too, like Delete.
        // Then Disable event can post itself instead of sending. Add check to
        // send_event too.
        return;
    }

    // Remove Enable/Disable pairs
    Event::Type type{event->type()};
    if (type == Event::Enable) {
        auto begin = std::begin(queue_);
        auto end = std::end(queue_);
        auto is_disable_event =
            [&event](const std::unique_ptr<Event>& on_queue) {
                bool result{event->receiver() == on_queue->receiver() &&
                            on_queue->type() == Event::Disable};
                return result;
            };
        auto erase_iter = std::remove_if(begin, end, is_disable_event);
        if (erase_iter != end) {
            queue_.erase(erase_iter, end);
            return;
        }
    } else if (type == Event::Disable) {
        auto begin = std::begin(queue_);
        auto end = std::end(queue_);
        auto is_enable_event =
            [&event](const std::unique_ptr<Event>& on_queue) {
                bool result{event->receiver() == on_queue->receiver() &&
                            on_queue->type() == Event::Enable};
                return result;
            };
        auto erase_iter = std::remove_if(begin, end, is_enable_event);
        if (erase_iter != end) {
            queue_.erase(erase_iter, end);
            return;
        }
    }

    // Optimize out expensive duplicate events.
    if (type == Event::Paint || type == Event::Move || type == Event::Resize ||
        type == Event::Disable || type == Event::Enable) {
        auto begin = std::begin(queue_);
        auto end = std::end(queue_);
        auto events_equal = [&event](const std::unique_ptr<Event>& on_queue) {
            bool result{*event == *on_queue};
            return result;
        };
        queue_.erase(std::remove_if(begin, end, events_equal), end);
        if (type == Event::Delete) {
            remove_descendant_delete_events(*event, queue_);
        }
    }
    queue_.emplace_back(std::move(event));
}

}  // namespace detail
}  // namespace cppurses
