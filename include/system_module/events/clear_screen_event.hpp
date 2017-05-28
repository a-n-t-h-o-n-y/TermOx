#ifndef SYSTEM_MODULE_EVENTS_CLEAR_SCREEN_EVENT_HPP
#define SYSTEM_MODULE_EVENTS_CLEAR_SCREEN_EVENT_HPP

#include "system_module/event.hpp"

namespace cppurses {

class Clear_screen_event : public Event {
   public:
    Clear_screen_event() : Event{Event::ClearScreen} {}
};

}  // namespace cppurses
#endif  // SYSTEM_MODULE_EVENTS_CLEAR_SCREEN_EVENT_HPP
