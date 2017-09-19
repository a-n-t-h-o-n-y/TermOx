#include "system/events/child_event.hpp"
#include "widget/widget.hpp"

namespace cppurses {

// class Child_event
Child_event::Child_event(Event::Type type, Widget* receiver, Widget* child)
    : Event{type, receiver}, child_{child} {}

// class Child_added_event
Child_added_event::Child_added_event(Widget* receiver, Widget* child)
    : Child_event{Event::ChildAdded, receiver, child} {}

bool Child_added_event::send() const {
    return receiver_->child_added_event(child_);
}

bool Child_added_event::filter_send(Widget* filter_widget) const {
    return filter_widget->child_added_event_filter(receiver_, child_);
}

// class Child_removed_event
Child_removed_event::Child_removed_event(Widget* receiver, Widget* child)
    : Child_event{Event::ChildRemoved, receiver, child} {}

bool Child_removed_event::send() const {
    return receiver_->child_removed_event(child_);
}

bool Child_removed_event::filter_send(Widget* filter_widget) const {
    return filter_widget->child_removed_event_filter(receiver_, child_);
}

// class Child_polished_event
Child_polished_event::Child_polished_event(Widget* receiver, Widget* child)
    : Child_event{Event::ChildPolished, receiver, child} {}

bool Child_polished_event::send() const {
    return receiver_->child_polished_event(child_);
}

bool Child_polished_event::filter_send(Widget* filter_widget) const {
    return filter_widget->child_polished_event_filter(receiver_, child_);
}

}  // namespace cppurses
