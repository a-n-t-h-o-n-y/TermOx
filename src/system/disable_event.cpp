#include <cppurses/system/events/disable_event.hpp>

#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
Disable_event::Disable_event(Widget* receiver)
    : Event{Event::Disable, receiver} {}

bool Disable_event::send() const {
    receiver_->screen_state().tiles.clear();
    return receiver_->disable_event();
}

bool Disable_event::filter_send(Widget* filter) const {
    return filter->disable_event_filter(receiver_);
}

}  // namespace cppurses
