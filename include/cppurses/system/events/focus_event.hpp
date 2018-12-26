#ifndef CPPURSES_SYSTEM_EVENTS_FOCUS_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_FOCUS_EVENT_HPP
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Sent to Widget that have just become the Widget in focus.
class Focus_in_event : public Event {
    explicit Focus_in_event(Widget& receiver)
        : Event{Event::FocusIn, receiver} {}

   public:
    bool send() const override { return receiver_.focus_in_event(); }

    bool filter_send(Widget& filter) const override {
        return filter.focus_in_event_filter(receiver_);
    }
    friend class Focus;
};

/// Sent to Widget that have just lost the focus.
class Focus_out_event : public Event {
    explicit Focus_out_event(Widget& receiver)
        : Event{Event::FocusOut, receiver} {}

   public:
    bool send() const override { return receiver_.focus_out_event(); }

    bool filter_send(Widget& filter) const override {
        return filter.focus_out_event_filter(receiver_);
    }
    friend class Focus;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_FOCUS_EVENT_HPP
