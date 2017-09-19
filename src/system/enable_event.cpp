#include "system/events/enable_event.hpp"
#include "widget/widget.hpp"

namespace cppurses {

Enable_event::Enable_event(Widget* receiver) : Event{Event::Enable, receiver} {}

bool Enable_event::send() const {
    return receiver_->enable_event();
}

bool Enable_event::filter_send(Widget* filter_widget) const {
    return filter_widget->enable_event_filter(receiver_);
}

}  // namespace cppurses
