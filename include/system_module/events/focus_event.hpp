#ifndef SYSTEM_MODULE_EVENTS_FOCUS_EVENT_HPP
#define SYSTEM_MODULE_EVENTS_FOCUS_EVENT_HPP

#include "system_module/event.hpp"

namespace twf {

class Focus_event : public Event {
   public:
    explicit Focus_event(Event::Type type) : Event{type} {}
};

}  // namespace twf
#endif  // SYSTEM_MODULE_EVENTS_FOCUS_EVENT_HPP
