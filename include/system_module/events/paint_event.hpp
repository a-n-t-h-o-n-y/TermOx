#ifndef SYSTEM_MODULE_EVENTS_PAINT_EVENT_HPP
#define SYSTEM_MODULE_EVENTS_PAINT_EVENT_HPP

#include "system_module/event.hpp"

namespace twf {

class Paint_event : public Event {
   public:
    Paint_event() : Event{Event::Paint} {}
};

}  // namespace twf
#endif  // SYSTEM_MODULE_EVENTS_PAINT_EVENT_HPP
