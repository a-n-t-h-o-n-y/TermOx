#include <mcurses/system_module/events/child_event.hpp>
#include <mcurses/system_module/event.hpp>

namespace mcurses {

Child_event::Child_event(Event::Type type, Object* child)
:Event(type), child_{child}
{}

bool Child_event::added() const
{
	return (type_ == Event::Type::ChildAdded) ? true : false;
}

bool Child_event::polished() const
{
	return (type_ == Event::Type::ChildPolished) ? true : false;
}

bool Child_event::removed() const
{
	return (type_ == Event::Type::ChildRemoved) ? true : false;
}

} // namespace mcurses