#ifndef SYSTEM_EVENTS_FOCUS_EVENT_HPP
#define SYSTEM_EVENTS_FOCUS_EVENT_HPP

#include "system/event.hpp"

namespace cppurses {

class Focus_event : public Event {
   public:
    explicit Focus_event(Event::Type type) : Event{type} {}
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_FOCUS_EVENT_HPP
