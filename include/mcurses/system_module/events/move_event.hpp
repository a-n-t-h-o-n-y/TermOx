#ifndef MOVE_EVENT_HPP
#define MOVE_EVENT_HPP

#include "../event.hpp"

namespace mcurses {

class Move_event : public Event {
public:
	Move_event(unsigned old_x, unsigned old_y, unsigned new_x, unsigned new_y);
	
	unsigned old_x() const { return old_x_; }
	unsigned old_y() const { return old_y_; }
	unsigned new_x() const { return new_x_; }
	unsigned new_y() const { return new_y_; }

protected:
	unsigned old_x_;
	unsigned old_y_;
	unsigned new_x_;
	unsigned new_y_;
};

} // namespace mcurses
#endif // MOVE_EVENT_HPP