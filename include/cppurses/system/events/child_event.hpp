#ifndef CPPURSES_SYSTEM_EVENTS_CHILD_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_CHILD_EVENT_HPP
#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
class Widget;

/// Base class for child events.
class Child_event : public Event {
   public:
    Child_event(Event::Type type, Widget& receiver, Widget& child)
        : Event{type, receiver}, child_{child} {}

    bool send() const override {
        receiver_.screen_state().optimize.child_event = true;
        return true;
    }

   protected:
    Widget& child_;
};

/// Event sent to a parent Widget when a child is added.
class Child_added_event : public Child_event {
   public:
    Child_added_event(Widget& receiver, Widget& child)
        : Child_event{Event::ChildAdded, receiver, child} {}

    bool send() const override {
        Child_event::send();
        return receiver_.child_added_event(child_);
    }

    bool filter_send(Widget& filter) const override {
        return filter.child_added_event_filter(receiver_, child_);
    }
};

/// Event sent to a parent Widget when a child is removed.
class Child_removed_event : public Child_event {
   public:
    Child_removed_event(Widget& receiver, Widget& child)
        : Child_event{Event::ChildRemoved, receiver, child} {}

    bool send() const override {
        Child_event::send();
        return receiver_.child_removed_event(child_);
    }

    bool filter_send(Widget& filter) const override {
        return filter.child_removed_event_filter(receiver_, child_);
    }
};

/// Event sent to a parent Widget when a child has its size_policy modified.
class Child_polished_event : public Child_event {
   public:
    Child_polished_event(Widget& receiver, Widget& child)
        : Child_event{Event::ChildPolished, receiver, child} {}

    bool send() const override {
        Child_event::send();
        return receiver_.child_polished_event(child_);
    }

    bool filter_send(Widget& filter) const override {
        return filter.child_polished_event_filter(receiver_, child_);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_CHILD_EVENT_HPP
