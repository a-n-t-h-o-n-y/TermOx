#include "system/events/show_event.hpp"
#include "widget/widget.hpp"

namespace cppurses {

Show_event::Show_event(Widget* receiver) : Event{Event::Show, receiver} {}

bool Show_event::send() const {
    return receiver_->show_event();
}

bool Show_event::filter_send(Widget* filter_widget) const {
    return filter_widget->show_event_filter(receiver_);
}

}  // namespace cppurses
