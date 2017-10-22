#include <cppurses/system/events/focus_event.hpp>
#include <cppurses/system/event_handler.hpp>

namespace cppurses {

Focus_in_event::Focus_in_event(Event_handler* receiver)
    : Event{Event::FocusIn, receiver} {}

bool Focus_in_event::send() const {
    return receiver_->focus_in_event();
}

bool Focus_in_event::filter_send(Event_handler* filter) const {
    return filter->focus_in_event_filter(receiver_);
}

Focus_out_event::Focus_out_event(Event_handler* receiver)
    : Event{Event::FocusOut, receiver} {}

bool Focus_out_event::send() const {
    return receiver_->focus_out_event();
}

bool Focus_out_event::filter_send(Event_handler* filter) const {
    return filter->focus_out_event_filter(receiver_);
}

}  // namespace cppurses
