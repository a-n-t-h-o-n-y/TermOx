#ifndef SHOW_EVENT_HPP
#define SHOW_EVENT_HPP

#include "../event.hpp"

namespace twf {

class Show_event : public Event {
   public:
    Show_event() : Event{Event::Type::Show} {}
};

}  // namespace twf
#endif  // SHOW_EVENT_HPP
