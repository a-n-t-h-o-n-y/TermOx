#ifndef CLEAR_SCREEN_EVENT_HPP
#define CLEAR_SCREEN_EVENT_HPP

#include "../event.hpp"

namespace twf {

class Clear_screen_event : public Event {
   public:
    Clear_screen_event() : Event{Event::Type::ClearScreen} {}
};

}  // namespace twf
#endif  // CLEAR_SCREEN_EVENT_HPP
