#ifndef SYSTEM_EVENTS_DEFERRED_DELETE_EVENT_HPP
#define SYSTEM_EVENTS_DEFERRED_DELETE_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Event_handler;
class Widget;

class Deferred_delete_event : public Event {
   public:
    explicit Deferred_delete_event(Event_handler* receiver);

    Widget* to_delete() const;

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;

   private:
    Event_handler* to_delete_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_DEFERRED_DELETE_EVENT_HPP
