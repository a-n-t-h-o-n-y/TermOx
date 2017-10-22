#include <cppurses/system/events/child_event.hpp>
#include <cppurses/system/event_handler.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

// class Child_event
Child_event::Child_event(Event::Type type,
                         Event_handler* receiver,
                         Widget* child)
    : Event{type, receiver}, child_{child} {}

// class Child_added_event
Child_added_event::Child_added_event(Event_handler* receiver, Widget* child)
    : Child_event{Event::ChildAdded, receiver, child} {}

bool Child_added_event::send() const {
    return receiver_->child_added_event(child_);
}

bool Child_added_event::filter_send(Event_handler* filter) const {
    return filter->child_added_event_filter(receiver_, child_);
}

// class Child_removed_event
Child_removed_event::Child_removed_event(Event_handler* receiver, Widget* child)
    : Child_event{Event::ChildRemoved, receiver, child} {}

bool Child_removed_event::send() const {
    return receiver_->child_removed_event(child_);
}

bool Child_removed_event::filter_send(Event_handler* filter) const {
    return filter->child_removed_event_filter(receiver_, child_);
}

// class Child_polished_event
Child_polished_event::Child_polished_event(Event_handler* receiver,
                                           Widget* child)
    : Child_event{Event::ChildPolished, receiver, child} {}

bool Child_polished_event::send() const {
    return receiver_->child_polished_event(child_);
}

bool Child_polished_event::filter_send(Event_handler* filter) const {
    return filter->child_polished_event_filter(receiver_, child_);
}

}  // namespace cppurses
