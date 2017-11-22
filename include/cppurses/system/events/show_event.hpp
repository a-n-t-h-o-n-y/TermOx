#ifndef SYSTEM_EVENTS_SHOW_EVENT_HPP
#define SYSTEM_EVENTS_SHOW_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Event_handler;

class Show_event : public Event {
   public:
    explicit Show_event(Event_handler* receiver);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_SHOW_EVENT_HPP
