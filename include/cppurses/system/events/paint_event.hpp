#ifndef CPPURSES_SYSTEM_EVENTS_PAINT_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_PAINT_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Event_handler;

class Paint_event : public Event {
   public:
    explicit Paint_event(Event_handler* receiver);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_PAINT_EVENT_HPP
