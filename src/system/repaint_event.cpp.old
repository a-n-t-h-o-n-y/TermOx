#include <cppurses/system/events/repaint_event.hpp>

#include <cppurses/painter/detail/is_not_paintable.hpp>
#include <cppurses/system/event_handler.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Repaint_event::Repaint_event(Event_handler* receiver)
    : Event{Event::Repaint, receiver} {}

bool Repaint_event::send() const {
    Widget* widg{static_cast<Widget*>(receiver_)};
    if (detail::is_not_paintable(widg)) {
        return false;
    }
    return receiver_->repaint_event();
}

bool Repaint_event::filter_send(Event_handler* filter) const {
    return filter->repaint_event_filter(receiver_);
}

}  // namespace cppurses
