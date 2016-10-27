#ifndef MOVE_EVENT_HPP
#define MOVE_EVENT_HPP

#include "../event.hpp"

namespace mcurses {

class Move_event : public Event {
public:
	Move_event(unsigned new_x, unsigned new_y, unsigned old_x = 0, unsigned old_y = 0);
	
	unsigned new_x() const { return new_x_; }
	unsigned new_y() const { return new_y_; }
	unsigned old_x() const { return old_x_; }
	unsigned old_y() const { return old_y_; }

protected:
	unsigned new_x_;
	unsigned new_y_;
	unsigned old_x_;
	unsigned old_y_;
};

} // namespace mcurses
#endif // MOVE_EVENT_HPP