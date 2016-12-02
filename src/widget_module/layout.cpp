#include <mcurses/widget_module/layout.hpp>
#include <mcurses/system_module/object.hpp>

namespace mcurses {

Layout::Layout()
{
	// Set dimensions to maximum initially
	this->set_x(0);
	this->set_y(0);
	this->geometry().set_width(System::max_width());
	this->geometry().set_height(System::max_height());

	this->initialize();
}

void Layout::initialize()
{
	// Slots
	this->update_layout = std::bind(&Layout::update_geometry, this);
	this->update_layout.track(this->destroyed);
}

void Layout::update()
{
	this->update_geometry();
	Widget::update();
}

void Layout::child_event(Child_event& event)
{
	this->update();
	event.accept();
}

} // namespace mcurses
