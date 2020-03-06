#ifndef CPPURSES_SYSTEM_EVENTS_ENABLE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_ENABLE_EVENT_HPP
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Sent to a Widget that has been enabled.
/** Enabled Widget receive Events and are painted to the screen. */
class Enable_event : public Event {
   public:
    explicit Enable_event(Widget& receiver) : Event{Event::Enable, receiver} {}

    auto send() const -> bool override { return receiver_.enable_event(); }

    auto filter_send(Widget& filter) const -> bool override
    {
        return filter.enable_event_filter(receiver_);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_ENABLE_EVENT_HPP
