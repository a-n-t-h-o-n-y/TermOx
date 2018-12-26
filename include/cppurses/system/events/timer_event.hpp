#ifndef CPPURSES_SYSTEM_EVENTS_TIMER_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_TIMER_EVENT_HPP
#include <cppurses/painter/detail/is_paintable.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
class Widget;

class Timer_event : public Event {
   public:
    Timer_event(Widget& receiver) : Event{Event::Timer, receiver} {}

    bool send() const override {
        return detail::is_paintable(receiver_) ? receiver_.timer_event() : true;
    }

    bool filter_send(Widget& filter) const override {
        return filter.timer_event_filter(receiver_);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_TIMER_EVENT_HPP
