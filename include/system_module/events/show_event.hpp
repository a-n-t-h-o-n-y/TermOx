#ifndef SYSTEM_MODULE_EVENTS_SHOW_EVENT_HPP
#define SYSTEM_MODULE_EVENTS_SHOW_EVENT_HPP

#include "system_module/event.hpp"

namespace twf {

class Show_event : public Event {
   public:
    Show_event() : Event{Event::Show} {}
};

}  // namespace twf
#endif  // SYSTEM_MODULE_EVENTS_SHOW_EVENT_HPP
