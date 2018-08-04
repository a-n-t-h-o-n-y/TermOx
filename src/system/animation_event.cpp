#include <cppurses/system/events/animation_event.hpp>

#include <cppurses/painter/detail/is_not_paintable.hpp>
#include <cppurses/system/event_handler.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/system/system.hpp>

namespace cppurses {

Animation_event::Animation_event(Event_handler* receiver)
    : Event{Event::Animation, receiver} {}

bool Animation_event::send() const {
    Widget* widg{static_cast<Widget*>(receiver_)};
    if (detail::is_not_paintable(widg)) {
        return true;
    }
    bool result{receiver_->animation_event()};
    return result;
}

bool Animation_event::filter_send(Event_handler* filter) const {
    return filter->animation_event_filter(receiver_);
}

}  // namespace cppurses
