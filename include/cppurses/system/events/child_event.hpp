#ifndef SYSTEM_EVENTS_CHILD_EVENT_HPP
#define SYSTEM_EVENTS_CHILD_EVENT_HPP
#include "system/event.hpp"

namespace cppurses {
class Widget;

class Child_event : public Event {
   public:
    Child_event(Event::Type type, Widget* receiver, Widget* child);

   protected:
    Widget* child_;
};

class Child_added_event : public Child_event {
   public:
    Child_added_event(Widget* receiver, Widget* child);
    bool send() const override;
    bool filter_send(Widget* filter_widget) const override;
};

class Child_removed_event : public Child_event {
   public:
    Child_removed_event(Widget* receiver, Widget* child);
    bool send() const override;
    bool filter_send(Widget* filter_widget) const override;
};

class Child_polished_event : public Child_event {
   public:
    Child_polished_event(Widget* receiver, Widget* child);
    bool send() const override;
    bool filter_send(Widget* filter_widget) const override;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_CHILD_EVENT_HPP
