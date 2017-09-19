#include "system/events/paint_event.hpp"
#include "widget/widget.hpp"

namespace cppurses {
Paint_event::Paint_event(Widget* receiver) : Event{Event::Paint, receiver} {}

bool Paint_event::send() const {
    if (receiver_->visible()) {
        return receiver_->paint_event();
    }
    return false;
}

bool Paint_event::filter_send(Widget* filter_widget) const {
    return filter_widget->paint_event_filter(receiver_);
}

}  // namespace cppurses
