#include "system/events/on_tree_event.hpp"
#include "system/event_handler.hpp"

namespace cppurses {

On_tree_event::On_tree_event(Event_handler* receiver, bool on_tree)
    : Event{Event::OnTree, receiver}, on_tree_{on_tree} {}

bool On_tree_event::send() const {
    return receiver_->on_tree_event(on_tree_);
}

bool On_tree_event::filter_send(Event_handler* filter) const {
    return filter->on_tree_event_filter(receiver_, on_tree_);
}

}  // namespace cppurses
