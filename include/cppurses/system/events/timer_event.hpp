#ifndef CPPURSES_SYSTEM_EVENTS_TIMER_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_TIMER_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Widget;

class Timer_event : public Event {
   public:
    Timer_event(Widget& receiver);

    bool send() const override;

    bool filter_send(Widget& filter) const override;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_TIMER_EVENT_HPP
