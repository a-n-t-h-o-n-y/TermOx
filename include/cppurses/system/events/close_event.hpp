#ifndef SYSTEM_EVENTS_CLOSE_EVENT_HPP
#define SYSTEM_EVENTS_CLOSE_EVENT_HPP

#include "system/event.hpp"

namespace cppurses {

class Close_event : public Event {
   public:
    Close_event() : Event{Event::Close} {}
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_CLOSE_EVENT_HPP
