#ifndef SYSTEM_EVENTS_CHILD_EVENT_HPP
#define SYSTEM_EVENTS_CHILD_EVENT_HPP
#include "system/event.hpp"

namespace cppurses {
class Event_handler;
class Widget;

class Child_event : public Event {
   public:
    Child_event(Event::Type type, Event_handler* receiver, Widget* child);

   protected:
    Widget* child_;
};

class Child_added_event : public Child_event {
   public:
    Child_added_event(Event_handler* receiver, Widget* child);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

class Child_removed_event : public Child_event {
   public:
    Child_removed_event(Event_handler* receiver, Widget* child);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

class Child_polished_event : public Child_event {
   public:
    Child_polished_event(Event_handler* receiver, Widget* child);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_CHILD_EVENT_HPP
