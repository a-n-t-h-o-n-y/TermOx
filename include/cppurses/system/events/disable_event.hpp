#ifndef CPPURSES_SYSTEM_EVENTS_DISABLE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_DISABLE_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Event_handler;

/// Sent to a Widget that has been disabled.
/** Disabled Widgets do not receive Events(except for Disable_events), and are
 *  not painted to the screen. */
class Disable_event : public Event {
   public:
    explicit Disable_event(Event_handler* receiver);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_DISABLE_EVENT_HPP
