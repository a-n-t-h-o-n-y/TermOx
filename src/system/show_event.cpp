#include "system/events/show_event.hpp"
#include "system/event_handler.hpp"

namespace cppurses {

Show_event::Show_event(Event_handler* receiver)
    : Event{Event::Show, receiver} {}

bool Show_event::send() const {
    return receiver_->show_event();
}

bool Show_event::filter_send(Event_handler* filter) const {
    return filter->show_event_filter(receiver_);
}

}  // namespace cppurses
