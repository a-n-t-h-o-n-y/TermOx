#ifndef CPPURSES_SYSTEM_EVENTS_TERMINAL_RESIZE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_TERMINAL_RESIZE_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Widget;

class Terminal_resize_event : public Event {
   public:
    Terminal_resize_event(Widget& receiver)
        : Event{Event::TerminalResize, receiver} {}

    bool send() const override;

    bool filter_send(Widget& filter) const override { return true; }
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_TERMINAL_RESIZE_EVENT_HPP
