#include <mcurses/system_module/object.hpp>
#include <mcurses/system_module/event.hpp>
#include <mcurses/system_module/events/child_event.hpp>
#include <mcurses/system_module/events/enable_event.hpp>
#include <mcurses/system_module/system.hpp>
#include <mcurses/signal_module/slot.hpp>
#include <mcurses/widget_module/widget.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>

namespace mcurses {

Object::~Object()
{
	Child_event ev{Event::Type::ChildRemoved, this};
	System::send_event(this->parent(), ev);
	destroyed(this);
}

void Object::initialize()
{
	// Slots
	this->delete_later = [this](){ System::post_event(this, std::make_unique<Event>(Event::Type::DeferredDelete));};
	this->delete_later.track(this->destroyed); // Hack to disable slot when *this dies.

	this->enable = [this](){this->set_enabled(true);};
	this->enable.track(this->destroyed);

	this->disable = [this](){this->set_enabled(false);};
	this->disable.track(this->destroyed);
}

void Object::add_child(std::unique_ptr<Object> child)
{
	children_.emplace_back(std::move(child));
	children_.back()->set_parent(this);
	Child_event ev(Event::Type::ChildAdded, this);
	System::send_event(this, ev);
	return;
}

void Object::delete_child(Object* child) {
	auto at = std::find_if(std::begin(children_), std::end(children_),
		[&child](auto& c){return c.get() == child ? true : false;});
	if(at != std::end(children_)) {
		children_.erase(at);
	}
	return;
}

bool Object::event(Event& event)
{
	if(event.type() == Event::Type::ChildAdded
	|| event.type() == Event::Type::ChildPolished
	|| event.type() == Event::Type::ChildRemoved) {
		this->child_event(static_cast<Child_event&>(event));
		return event.is_accepted();
	}

	if(event.type() == Event::Type::EnabledChange) {
		this->enable_event(static_cast<Enable_event&>(event));
		return event.is_accepted();
	}

	return event.is_accepted();
}

void Object::child_event(Child_event& event)
{
	Widget* parent = dynamic_cast<Widget*>(this->parent());
	if(parent) {
		parent->update();
	}
	event.accept();
	return;
}

void Object::enable_event(Enable_event& event){
	event.accept();
	return;
}

bool Object::event_filter(Object* watched, Event& event)
{
	return false;
}

void Object::install_event_filter(Object* filter_object)
{
	if(filter_object == this) { return; }
	event_filter_objects_.push_back(filter_object);
	return;
}

Object* Object::parent() const
{
	return parent_;
}

void Object::remove_event_filter(Object* obj)
{
	auto at = std::find(std::begin(event_filter_objects_), std::end(event_filter_objects_), obj);
	if(at != std::end(event_filter_objects_)) {
		event_filter_objects_.erase(at);
	}
	return;
}

void Object::set_name(const std::string& name)
{
	object_name_ = name;
	object_name_changed(name);
	return;
}

void Object::set_parent(Object* parent)
{
	parent_ = parent;
	return;
}

void Object::set_enabled(bool enabled)
{
	enabled_ = enabled;
	Enable_event ee(enabled);
	System::send_event(this, ee);
	return;
}

std::vector<Object*> Object::children() const
{
	std::vector<Object*> ret;
	std::transform(std::begin(children_), std::end(children_), std::back_inserter(ret), [](auto& up){return up.get();});
	return ret;
}

} // namespace mcurses