#include "system/events/disable_event.hpp"
#include "widget/widget.hpp"

namespace cppurses {
Disable_event::Disable_event(Widget* receiver)
    : Event{Event::Disable, receiver} {}

bool Disable_event::send() const {
    return receiver_->disable_event();
}

bool Disable_event::filter_send(Widget* filter_widget) const {
    return filter_widget->disable_event_filter(receiver_);
}

}  // namespace cppurses
