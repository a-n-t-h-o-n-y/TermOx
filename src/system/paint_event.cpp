#include <cppurses/system/events/paint_event.hpp>

#include <cppurses/painter/painter.hpp>
#include <cppurses/system/event_handler.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
Paint_event::Paint_event(Event_handler* receiver)
    : Event{Event::Paint, receiver},
      painter_{static_cast<Widget*>(receiver_)} {}

bool Paint_event::send() const {
    Widget* widg{static_cast<Widget*>(receiver_)};
    if (widg->visible()) {
        return receiver_->paint_event(painter_);
    }
    return false;
}

bool Paint_event::filter_send(Event_handler* filter) const {
    return filter->paint_event_filter(receiver_, painter_);
}

}  // namespace cppurses
