#ifndef FOCUS_EVENT_HPP
#define FOCUS_EVENT_HPP

#include "../event.hpp"

namespace mcurses {

class Focus_event : public Event {
   public:
    explicit Focus_event(Event::Type type) : Event{type} {}
};

}  // namespace mcurses
#endif  // FOCUS_EVENT_HPP
