#ifndef SYSTEM_EVENTS_SHOW_EVENT_HPP
#define SYSTEM_EVENTS_SHOW_EVENT_HPP

#include "system/event.hpp"

namespace cppurses {

class Show_event : public Event {
   public:
    Show_event() : Event{Event::Show} {}
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_SHOW_EVENT_HPP
