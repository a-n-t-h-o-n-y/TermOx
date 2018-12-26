#ifndef CPPURSES_SYSTEM_EVENTS_DISABLE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_DISABLE_EVENT_HPP
#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Sent to a Widget that has been disabled.
/** Disabled Widgets do not receive Events(except for Disable_events), and are
 *  not painted to the screen. */
class Disable_event : public Event {
   public:
    explicit Disable_event(Widget& receiver)
        : Event{Event::Disable, receiver} {}

    bool send() const override {
        receiver_.screen_state().tiles.clear();
        return receiver_.disable_event();
    }

    bool filter_send(Widget& filter) const override {
        return filter.disable_event_filter(receiver_);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_DISABLE_EVENT_HPP
