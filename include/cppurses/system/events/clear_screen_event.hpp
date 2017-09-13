#ifndef SYSTEM_EVENTS_CLEAR_SCREEN_EVENT_HPP
#define SYSTEM_EVENTS_CLEAR_SCREEN_EVENT_HPP

#include "system/event.hpp"

namespace cppurses {

class Clear_screen_event : public Event {
   public:
    Clear_screen_event() : Event{Event::ClearScreen} {}
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_CLEAR_SCREEN_EVENT_HPP
