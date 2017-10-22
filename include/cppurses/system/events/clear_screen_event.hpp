#ifndef SYSTEM_EVENTS_CLEAR_SCREEN_EVENT_HPP
#define SYSTEM_EVENTS_CLEAR_SCREEN_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Event_handler;

class Clear_screen_event : public Event {
   public:
    explicit Clear_screen_event(Event_handler* receiver);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_CLEAR_SCREEN_EVENT_HPP
