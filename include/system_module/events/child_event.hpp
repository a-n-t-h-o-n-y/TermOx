#ifndef CHILD_EVENT_HPP
#define CHILD_EVENT_HPP

#include "../event.hpp"

namespace mcurses {

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

}  // namespace mcurses
#endif  // CHILD_EVENT_HPP
