#include <mcurses/system_module/object.hpp>
#include <mcurses/system_module/event.hpp>

namespace mcurses {

bool Object::event(const Event& event)
{
	return true;
}

Object* Object::parent() const
{
	return parent_;
}

} // namespace mcurses