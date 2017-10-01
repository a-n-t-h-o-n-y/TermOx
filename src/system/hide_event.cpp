#include "system/events/hide_event.hpp"
#include "system/event_handler.hpp"

namespace cppurses {

Hide_event::Hide_event(Event_handler* receiver) : Event{Event::Hide, receiver} {}

bool Hide_event::send() const {
    return receiver_->hide_event();
}

bool Hide_event::filter_send(Event_handler* filter) const {
    return filter->hide_event_filter(receiver_);
}

}  // namespace cppurses
