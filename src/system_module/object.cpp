#include <mcurses/system_module/object.hpp>
#include <mcurses/system_module/event.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <queue>
#include <string>

namespace mcurses {

std::vector<Object*> Object::children() const
{
	std::vector<Object*> ret;
	std::transform(std::begin(children_), std::end(children_), std::back_inserter(ret), [](auto& up){return up.get();});
	return ret;
}

void Object::add_child(std::unique_ptr<Object> child)
{
	children_.emplace_back(std::move(child));
}

bool Object::event(const Event& event)
{
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
	// Anything?
	return true;
}

// Breadth First Search for name
template <typename T>
T* Object::find_child(const std::string& name) const
{
	std::queue<const Object*> queue_;
	queue_.push(this);
	while(!queue_.empty()) {
		const Object* current =queue_.front();
		queue_.pop();
		if(current->name() == name && dynamic_cast<T*>(current)) {
			return dynamic_cast<T*>(current);
		}
		std::for_each(std::begin(current->children()), std::end(current->children()), [&](Object* p){queue_.push(p);});
	}
	return nullptr;
}




Object* Object::parent() const
{
	return parent_;
}




} // namespace mcurses