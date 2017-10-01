#ifndef SYSTEM_EVENTS_CLOSE_EVENT_HPP
#define SYSTEM_EVENTS_CLOSE_EVENT_HPP
#include "system/event.hpp"

namespace cppurses {
class Event_handler;

class Close_event : public Event {
   public:
    explicit Close_event(Event_handler* receiver);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_CLOSE_EVENT_HPP
