#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "event.hpp"
#include "events/child_event.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <string>

namespace mcurses
{

class Object {
public:
	Object(){}
	Object(const std::string& name): object_name_{name}{}
	virtual ~Object(){/* destroyed(); */}

	template <typename T, typename ... Args>
	T& make_child(Args&&... args) {
		children_.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
		return static_cast<T&>(*children_.back());
	}

	void add_child(std::unique_ptr<Object> child);

	virtual bool event(const Event& event);

	virtual bool event_filter(Object* watched, const Event& event);

	template <typename T>
	T* find_child(const std::string& name) const;



	Object* parent() const;
	std::vector<Object*> children() const;

	virtual bool has_coordinates(unsigned glob_x, unsigned glob_y){ return false; }
	bool is_enabled() const { return enabled_; }
	std::string name() const { return object_name_; }
	void set_name(const std::string& s) { object_name_ = s; }
	
	// slot
	// delete_later();

protected:
	virtual void child_event(const Child_event& event);

	Object* parent_ = nullptr;
	std::vector<std::unique_ptr<Object>> children_;

	std::string object_name_;
	bool enabled_ = true;
};

} // namespace mcurses
#endif // OBJECT_HPP