#ifndef CLOSE_EVENT_HPP
#define CLOSE_EVENT_HPP

#include "../event.hpp"

namespace twf {

class Close_event : public Event {
   public:
    Close_event() : Event{Event::Type::Close} {}
};

}  // namespace twf
#endif  // CLOSE_EVENT_HPP
