#ifndef SYSTEM_EVENTS_DISABLE_EVENT_HPP
#define SYSTEM_EVENTS_DISABLE_EVENT_HPP
#include "system/event.hpp"

namespace cppurses {
class Event_handler;

class Disable_event : public Event {
   public:
    explicit Disable_event(Event_handler* receiver);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_DISABLE_EVENT_HPP
