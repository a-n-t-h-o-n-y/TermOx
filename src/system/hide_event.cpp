#include "system/events/hide_event.hpp"
#include "widget/widget.hpp"

namespace cppurses {

Hide_event::Hide_event(Widget* receiver) : Event{Event::Hide, receiver} {}

bool Hide_event::send() const {
    return receiver_->hide_event();
}

bool Hide_event::filter_send(Widget* filter_widget) const {
    return filter_widget->hide_event_filter(receiver_);
}

}  // namespace cppurses
