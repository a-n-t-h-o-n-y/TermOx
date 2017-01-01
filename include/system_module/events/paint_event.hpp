#ifndef PAINT_EVENT_HPP
#define PAINT_EVENT_HPP

#include "../event.hpp"

namespace twf {

class Paint_event : public Event {
   public:
    Paint_event() : Event{Event::Type::Paint} {}

   private:
};

}  // namespace twf
#endif  // PAINT_EVENT_HPP
