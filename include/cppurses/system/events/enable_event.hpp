#ifndef CPPURSES_SYSTEM_EVENTS_ENABLE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_ENABLE_EVENT_HPP
#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Sent to a Widget that has been enabled.
/** Enabled Widget receive Events and are painted to the screen. */
class Enable_event : public Event {
   public:
    explicit Enable_event(Widget& receiver) : Event{Event::Enable, receiver} {}

    bool send() const override {
        receiver_.screen_state().optimize.just_enabled = true;
        return receiver_.enable_event();
    }

    bool filter_send(Widget& filter) const override {
        return filter.enable_event_filter(receiver_);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_ENABLE_EVENT_HPP
