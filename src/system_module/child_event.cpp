#include "system_module/events/child_event.hpp"
#include "system_module/event.hpp"

namespace twf {

Child_event::Child_event(Event::Type type, Object* child)
    : Event{type}, child_{child} {}

bool Child_event::added() const {
    return type_ == Event::Type::ChildAdded;
}

bool Child_event::polished() const {
    return type_ == Event::Type::ChildPolished;
}

bool Child_event::removed() const {
    return type_ == Event::Type::ChildRemoved;
}

}  // namespace twf
