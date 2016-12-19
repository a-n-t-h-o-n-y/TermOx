#include <system_module/event.hpp>
#include <system_module/events/child_event.hpp>

namespace mcurses {

Child_event::Child_event(Event::Type type, Object* child)
    : Event(type), child_{child} {}

bool Child_event::added() const {
    return type_ == Event::Type::ChildAdded;
}

bool Child_event::polished() const {
    return type_ == Event::Type::ChildPolished;
}

bool Child_event::removed() const {
    return type_ == Event::Type::ChildRemoved;
}

}  // namespace mcurses
