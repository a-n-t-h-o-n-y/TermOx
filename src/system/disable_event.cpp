#include <cppurses/system/events/disable_event.hpp>
#include <cppurses/system/event_handler.hpp>

namespace cppurses {
Disable_event::Disable_event(Event_handler* receiver)
    : Event{Event::Disable, receiver} {}

bool Disable_event::send() const {
    return receiver_->disable_event();
}

bool Disable_event::filter_send(Event_handler* filter) const {
    return filter->disable_event_filter(receiver_);
}

}  // namespace cppurses
