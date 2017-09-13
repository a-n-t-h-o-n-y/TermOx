#ifndef SYSTEM_EVENTS_PAINT_EVENT_HPP
#define SYSTEM_EVENTS_PAINT_EVENT_HPP

#include "system/event.hpp"

namespace cppurses {

class Paint_event : public Event {
   public:
    Paint_event() : Event{Event::Paint} {}
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_PAINT_EVENT_HPP
