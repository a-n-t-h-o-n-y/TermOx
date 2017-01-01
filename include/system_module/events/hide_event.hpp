#ifndef HIDE_EVENT_HPP
#define HIDE_EVENT_HPP

#include "../event.hpp"

namespace twf {

class Hide_event : public Event {
   public:
    Hide_event() : Event{Event::Type::Hide} {}
};

}  // namespace twf
#endif  // HIDE_EVENT_HPP
