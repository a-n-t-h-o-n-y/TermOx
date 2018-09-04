#ifndef CPPURSES_SYSTEM_EVENTS_TERMINAL_RESIZE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_TERMINAL_RESIZE_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Event_handler;

class Terminal_resize_event : public Event {
   public:
    Terminal_resize_event(Event_handler* receiver);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_TERMINAL_RESIZE_EVENT_HPP
