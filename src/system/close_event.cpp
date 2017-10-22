#include <cppurses/system/events/close_event.hpp>
#include <cppurses/system/event_handler.hpp>

namespace cppurses {

Close_event::Close_event(Event_handler* receiver) : Event{Event::Close, receiver} {}

bool Close_event::send() const {
    return receiver_->close_event();
}

bool Close_event::filter_send(Event_handler* filter) const {
    return filter->close_event_filter(receiver_);
}

}  // namespace cppurses
