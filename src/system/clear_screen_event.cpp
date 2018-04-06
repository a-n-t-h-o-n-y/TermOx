#include <cppurses/system/events/clear_screen_event.hpp>

#include <cppurses/system/event_handler.hpp>

namespace cppurses {

Clear_screen_event::Clear_screen_event(Event_handler* receiver)
    : Event{Event::ClearScreen, receiver} {}

bool Clear_screen_event::send() const {
    return receiver_->clear_screen_event();
}

bool Clear_screen_event::filter_send(Event_handler* filter) const {
    return filter->clear_screen_event_filter(receiver_);
}

}  // namespace cppurses
