#include <cppurses/system/events/enable_event.hpp>

#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Enable_event::Enable_event(Widget* receiver) : Event{Event::Enable, receiver} {}

bool Enable_event::send() const {
    receiver_->screen_state().optimize.just_enabled = true;
    return receiver_->enable_event();
}

bool Enable_event::filter_send(Widget* filter) const {
    return filter->enable_event_filter(receiver_);
}

}  // namespace cppurses
