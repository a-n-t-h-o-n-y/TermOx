#ifndef SYSTEM_MODULE_EVENTS_PAINT_EVENT_HPP
#define SYSTEM_MODULE_EVENTS_PAINT_EVENT_HPP

#include "system_module/event.hpp"

namespace cppurses {

class Paint_event : public Event {
   public:
    Paint_event() : Event{Event::Paint} {}
};

}  // namespace cppurses
#endif  // SYSTEM_MODULE_EVENTS_PAINT_EVENT_HPP
