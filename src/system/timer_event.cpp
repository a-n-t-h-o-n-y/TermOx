#include <cppurses/system/events/timer_event.hpp>

#include <cppurses/painter/detail/is_not_paintable.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Timer_event::Timer_event(Widget* receiver) : Event{Event::Timer, receiver} {}

bool Timer_event::send() const {
    Widget* widg{static_cast<Widget*>(receiver_)};
    if (detail::is_not_paintable(widg)) {
        return true;
    }
    bool result{receiver_->timer_event()};
    return result;
}

bool Timer_event::filter_send(Widget* filter) const {
    return filter->timer_event_filter(receiver_);
}

}  // namespace cppurses
