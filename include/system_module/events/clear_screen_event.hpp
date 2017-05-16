#ifndef SYSTEM_MODULE_EVENTS_CLEAR_SCREEN_EVENT_HPP
#define SYSTEM_MODULE_EVENTS_CLEAR_SCREEN_EVENT_HPP

#include "system_module/event.hpp"

namespace twf {

class Clear_screen_event : public Event {
   public:
    Clear_screen_event() : Event{Event::ClearScreen} {}
};

}  // namespace twf
#endif  // SYSTEM_MODULE_EVENTS_CLEAR_SCREEN_EVENT_HPP
