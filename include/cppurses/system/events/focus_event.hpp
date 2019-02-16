#ifndef CPPURSES_SYSTEM_EVENTS_FOCUS_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_FOCUS_EVENT_HPP
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Sent to Widget that have just become the Widget in focus.
struct Focus_in_event : Event {
    explicit Focus_in_event(Widget& receiver)
        : Event{Event::FocusIn, receiver} {}

    bool send() const override { return receiver_.focus_in_event(); }

    bool filter_send(Widget& filter) const override {
        return filter.focus_in_event_filter(receiver_);
    }
};

/// Sent to Widget that have just lost the focus.
struct Focus_out_event : Event {
    explicit Focus_out_event(Widget& receiver)
        : Event{Event::FocusOut, receiver} {}

    bool send() const override { return receiver_.focus_out_event(); }

    bool filter_send(Widget& filter) const override {
        return filter.focus_out_event_filter(receiver_);
    }
};
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_FOCUS_EVENT_HPP
