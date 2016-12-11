#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <aml/signals/slot.hpp>
#include <aml/signals/signal.hpp>

#include <memory>
#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include <string>
#include <queue>

namespace mcurses {

class Event;
class Child_event;
class Enable_event;

class Object {
public:
	Object(){initialize();}
	Object(Object&&) = default;
	virtual ~Object();

	template <typename T, typename ... Args>
	T& make_child(Args&&... args) {
		this->add_child(std::make_unique<T>(std::forward<Args>(args)...));
		return static_cast<T&>(*children_.back());
	}

	void add_child(std::unique_ptr<Object> child);

	virtual bool event(Event& event);

	virtual bool event_filter(Object* watched, Event& event);

	// Breadth First Search for name
	template <typename T>
	T* find_child(const std::string& name)
	{
		std::queue<Object*> queue_;
		queue_.push(this);
		while(!queue_.empty()) {
			Object* current = queue_.front();
			queue_.pop();
			if(current->name() == name && dynamic_cast<T*>(current)) {
				return dynamic_cast<T*>(current);
			}
			auto children = current->children();
			std::for_each(std::begin(children), std::end(children), [&](Object* p){queue_.push(p);});
		}
		return nullptr;
	}

	template <typename T>
	const T* find_child(const std::string& name) const
	{
		return const_cast<Object*>(this)->find_child<T>(name);
	}

	void install_event_filter(Object* filter_object);

	std::string name() const { return object_name_; }

	Object* parent() const;

	void remove_event_filter(Object* obj);

	void set_name(const std::string& name);

	void set_parent(Object* parent);

	void set_enabled(bool enabled);

	std::vector<Object*> children() const;

	virtual bool has_coordinates(unsigned glob_x, unsigned glob_y){ return false; }
	
	bool is_enabled() const { return enabled_; }

	// Slots - change to uppercase Slot eventually
	Slot<void()> delete_later;
	Slot<void()> enable;
	Slot<void()> disable;

	// Signals
	Signal<void(Object*)> destroyed;
	Signal<void(const std::string&)> object_name_changed;

	friend class System;

protected:
	virtual void child_event(Child_event& event);
	virtual void enable_event(Enable_event& event);

	std::string object_name_;
	Object* parent_ = nullptr;
	std::vector<std::unique_ptr<Object>> children_;
	std::vector<Object*> event_filter_objects_;

	bool enabled_ = true;

private:
	void delete_child(Object* child);
	void initialize();
};

} // namespace mcurses
#endif // OBJECT_HPP
