#include "system/events/close_event.hpp"
#include "widget/widget.hpp"

namespace cppurses {

Close_event::Close_event(Widget* receiver) : Event{Event::Close, receiver} {}

bool Close_event::send() const {
    return receiver_->close_event();
}

bool Close_event::filter_send(Widget* filter_widget) const {
    return filter_widget->close_event_filter(receiver_);
}

}  // namespace cppurses
