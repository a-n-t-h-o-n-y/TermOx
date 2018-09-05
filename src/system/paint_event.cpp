#include <cppurses/system/events/paint_event.hpp>

#include <cppurses/painter/detail/is_not_paintable.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
Paint_event::Paint_event(Widget* receiver) : Event{Event::Paint, receiver} {}

bool Paint_event::send() const {
    if (detail::is_not_paintable(receiver_)) {
        return false;
    }
    return receiver_->paint_event();
}

bool Paint_event::filter_send(Widget* filter) const {
    return filter->paint_event_filter(receiver_);
}

}  // namespace cppurses
