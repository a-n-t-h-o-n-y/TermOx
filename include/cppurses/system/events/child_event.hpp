#ifndef CPPURSES_SYSTEM_EVENTS_CHILD_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_CHILD_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Widget;

/// Base class for child events.
class Child_event : public Event {
   public:
    Child_event(Event::Type type, Widget* receiver, Widget* child);
    bool send() const override;

   protected:
    Widget* child_;
};

/// Event sent to a parent Widget when a child is added.
class Child_added_event : public Child_event {
   public:
    Child_added_event(Widget* receiver, Widget* child);
    bool send() const override;
    bool filter_send(Widget* filter) const override;
};

/// Event sent to a parent Widget when a child is removed.
class Child_removed_event : public Child_event {
   public:
    Child_removed_event(Widget* receiver, Widget* child);
    bool send() const override;
    bool filter_send(Widget* filter) const override;
};

/// Event sent to a parent Widget when a child has its size_policy modified.
class Child_polished_event : public Child_event {
   public:
    Child_polished_event(Widget* receiver, Widget* child);
    bool send() const override;
    bool filter_send(Widget* filter) const override;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_CHILD_EVENT_HPP
