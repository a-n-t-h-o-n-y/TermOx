#ifndef CPPURSES_SYSTEM_EVENTS_CHILD_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_CHILD_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Widget;

/// Base class for child events.
class Child_event : public Event {
   public:
    Child_event(Event::Type type, Widget& receiver, Widget& child)
        : Event{type, receiver}, child_{child}
    {}

    auto send() const -> bool override;

   protected:
    Widget& child_;
};

/// Event sent to a parent Widget when a child is added.
class Child_added_event : public Child_event {
   public:
    Child_added_event(Widget& receiver, Widget& child)
        : Child_event{Event::ChildAdded, receiver, child}
    {}

    auto send() const -> bool override;

    auto filter_send(Widget& filter) const -> bool override;
};

/// Event sent to a parent Widget when a child is removed.
class Child_removed_event : public Child_event {
   public:
    Child_removed_event(Widget& receiver, Widget& child)
        : Child_event{Event::ChildRemoved, receiver, child}
    {}

    auto send() const -> bool override;

    auto filter_send(Widget& filter) const -> bool override;
};

/// Event sent to a parent Widget when a child has its size_policy modified.
class Child_polished_event : public Child_event {
   public:
    Child_polished_event(Widget& receiver, Widget& child)
        : Child_event{Event::ChildPolished, receiver, child}
    {}

    auto send() const -> bool override;

    auto filter_send(Widget& filter) const -> bool override;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_CHILD_EVENT_HPP
