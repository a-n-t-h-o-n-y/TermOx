#include <cppurses/system/detail/event_queue.hpp>

#include <algorithm>
#include <iterator>
#include <memory>

#include <cppurses/system/event.hpp>
#include <cppurses/system/events/deferred_delete_event.hpp>
#include <cppurses/widget/widget.hpp>

// #include <cppurses/system/detail/event_as_string.hpp>  // temp
// #include <utility/log.hpp>                             // temp
// #include <utility/type_info.hpp>                       // temp
// #include <thread>

namespace cppurses {
namespace detail {

void Event_queue::append(std::unique_ptr<Event> event) {
    if (event == nullptr || event->receiver() == nullptr) {
        return;
    }
    // Optimize out duplicate expensive events.
    Event::Type type = event->type();
    // This could almost be any duplicate event for the widget?
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
            remove_dd_children(*event);
        }
    }
    // utility::Log l;
    // l << "thread id: " << std::this_thread::get_id();
    // l << "\nEvent: " << detail::event_as_string(*event) << "\t|\t";
    // l << "Widget: " << event->receiver() << std::endl;
    queue_.emplace_back(std::move(event));
    // l << queue_.size() << '\n';
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
