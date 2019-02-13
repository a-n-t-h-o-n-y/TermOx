#include <cppurses/system/events/timer_event.hpp>

#include <cppurses/painter/detail/is_paintable.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Timer_event::Timer_event(Widget& receiver) : Event{Event::Timer, receiver} {}

bool Timer_event::send() const {
    return detail::is_paintable(receiver_) ? receiver_.timer_event() : true;
}

bool Timer_event::filter_send(Widget& filter) const {
    return filter.timer_event_filter(receiver_);
}
}  // namespace cppurses
