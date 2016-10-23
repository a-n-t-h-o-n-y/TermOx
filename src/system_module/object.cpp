#include <mcurses/system_module/object.hpp>
#include <mcurses/system_module/event.hpp>
#include <mcurses/system_module/events/child_event.hpp>
#include <mcurses/system_module/system.hpp>
#include <mcurses/signal_module/slot.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>

namespace mcurses {

void Object::initialize()
{
	delete_later = [this](){ System::post_event(this, std::make_unique<Event>(Event::Type::DeferredDelete));};
}

void Object::add_child(std::unique_ptr<Object> child)
{
	children_.emplace_back(std::move(child));
}

bool Object::event(const Event& event)
{
	bool handled_ = false;
	if(!event_filter_objects_.empty() && !handled_) {	// could use a range for and be cleaner?
		for_each(std::begin(event_filter_objects_), std::end(event_filter_objects_), [&](Object* p){ handled_ = p->event_filter(this, event); });
	}


	if(event.type() == Event::Type::ChildAdded
	|| event.type() == Event::Type::ChildPolished
	|| event.type() == Event::Type::ChildRemoved) {
		this->child_event(static_cast<const Child_event&>(event));
		return true;
	}
	return false;
}

void Object::child_event(const Child_event& event)
{
	// Does this have default behavior?
	return;
}

bool Object::event_filter(Object* watched, const Event& event)
{
	// Default to anything?
	return true;
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

std::vector<Object*> Object::children() const
{
	std::vector<Object*> ret;
	std::transform(std::begin(children_), std::end(children_), std::back_inserter(ret), [](auto& up){return up.get();});
	return ret;
}

} // namespace mcurses