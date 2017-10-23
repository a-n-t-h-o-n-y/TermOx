#include <cppurses/system/event_handler.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
Paint_event::Paint_event(Event_handler* receiver)
    : Event{Event::Paint, receiver} {}

bool Paint_event::send() const {
    if (static_cast<Widget*>(receiver_)->visible()) {
        return receiver_->paint_event();
    }
    return false;
}

bool Paint_event::filter_send(Event_handler* filter) const {
    return filter->paint_event_filter(receiver_);
}

}  // namespace cppurses
