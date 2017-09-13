#ifndef SYSTEM_EVENTS_CHILD_EVENT_HPP
#define SYSTEM_EVENTS_CHILD_EVENT_HPP

#include "system/event.hpp"

namespace cppurses {

class Child_event : public Event {
   public:
    Child_event(Event::Type type, Object* child);
    bool added() const;
    bool polished() const;
    bool removed() const;
    Object* child() const { return child_; }

   protected:
    Object* child_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_CHILD_EVENT_HPP
