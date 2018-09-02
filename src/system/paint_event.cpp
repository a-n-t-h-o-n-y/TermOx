#include <cppurses/system/events/paint_event.hpp>

#include <cppurses/painter/detail/is_not_paintable.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/event_handler.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
Paint_event::Paint_event(Event_handler* receiver)
    : Event{Event::Paint, receiver} {}

bool Paint_event::send() const {
    Widget* widg{static_cast<Widget*>(receiver_)};
    if (detail::is_not_paintable(widg)) {
        return false;
    }
    return receiver_->paint_event();
}

bool Paint_event::filter_send(Event_handler* filter) const {
    return filter->paint_event_filter(receiver_);
}

}  // namespace cppurses
