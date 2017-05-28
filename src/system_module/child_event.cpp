#include "system_module/events/child_event.hpp"
#include "system_module/event.hpp"

namespace cppurses {

Child_event::Child_event(Event::Type type, Object* child)
    : Event{type}, child_{child} {}

bool Child_event::added() const {
    return type_ == Event::ChildAdded;
}

bool Child_event::polished() const {
    return type_ == Event::ChildPolished;
}

bool Child_event::removed() const {
    return type_ == Event::ChildRemoved;
}

}  // namespace cppurses
