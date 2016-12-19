#ifndef PAINT_EVENT_HPP
#define PAINT_EVENT_HPP

#include "../event.hpp"

namespace mcurses {

class Paint_event : public Event {
   public:
    Paint_event() : Event{Event::Type::Paint} {}

   private:
};

}  // namespace mcurses
#endif  // PAINT_EVENT_HPP
