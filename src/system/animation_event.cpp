#include <cppurses/system/events/animation_event.hpp>

#include <cppurses/system/event_handler.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Animation_event::Animation_event(Event_handler* receiver)
    : Event{Event::Animation, receiver} {}

bool Animation_event::send() const {
    Widget* widg{static_cast<Widget*>(receiver_)};
    if (widg->width() == 0 || widg->height() == 0) {
        return true;
    }
    return receiver_->animation_event();
}

bool Animation_event::filter_send(Event_handler* filter) const {
    return filter->animation_event_filter(receiver_);
}

}  // namespace cppurses
