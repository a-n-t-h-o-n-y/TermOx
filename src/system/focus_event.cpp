#include "system/events/focus_event.hpp"
#include "widget/widget.hpp"

namespace cppurses {

Focus_in_event::Focus_in_event(Widget* receiver)
    : Event{Event::FocusIn, receiver} {}

bool Focus_in_event::send() const {
    return receiver_->focus_in_event();
}

bool Focus_in_event::filter_send(Widget* filter_widget) const {
    return filter_widget->focus_in_event_filter(receiver_);
}

Focus_out_event::Focus_out_event(Widget* receiver)
    : Event{Event::FocusOut, receiver} {}

bool Focus_out_event::send() const {
    return receiver_->focus_out_event();
}

bool Focus_out_event::filter_send(Widget* filter_widget) const {
    return filter_widget->focus_out_event_filter(receiver_);
}

}  // namespace cppurses
