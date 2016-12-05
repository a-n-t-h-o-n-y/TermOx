#include <system_module/event.hpp>
#include <system_module/system.hpp>

namespace mcurses {

Event::Event(Type type)
:type_{type}
{}

void Event::accept()
{
	accepted_ = true;
}

void Event::ignore()
{
	accepted_ = false;
}

bool Event::is_accepted() const
{
	return accepted_;
}

void Event::set_accepted(bool accept)
{
	accepted_ = accept;
}

Event::Type Event::type() const
{
	return type_;
}

bool operator==(const Event& x, const Event& y) {
	return x.type() == y.type();
}

} // namespace mcurses
