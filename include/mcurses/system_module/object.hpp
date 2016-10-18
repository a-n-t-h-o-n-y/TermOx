#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "event.hpp"

#include <vector>

namespace mcurses
{

class Object {
public:
	Object(){} // Delete this eventually
	Object(Object* parent):parent_{parent}{}
	virtual ~Object(){}

	virtual bool event(const Event& event);

	Object* parent() const;
	const std::vector<Object*>& children() const { return children_; }

	virtual bool has_coordinates(unsigned glob_x, unsigned glob_y){ return false; }
	bool is_enabled() const { return enabled_; }
	// slot
	// delete_later();

private:
	Object* parent_;
	std::vector<Object*> children_;

	// Properties
	bool enabled_ = true;
};

} // namespace mcurses
#endif // OBJECT_HPP