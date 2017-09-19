#include "system/events/clear_screen_event.hpp"
#include "widget/widget.hpp"

namespace cppurses {

Clear_screen_event::Clear_screen_event(Widget* receiver)
    : Event{Event::ClearScreen, receiver} {}

bool Clear_screen_event::send() const {
    return receiver_->clear_screen_event();
}

bool Clear_screen_event::filter_send(Widget* filter_widget) const {
    return filter_widget->clear_screen_event_filter(receiver_);
}

}  // namespace cppurses
