#ifndef RESIZE_EVENT_HPP
#define RESIZE_EVENT_HPP

#include "../event.hpp"

namespace mcurses {

class Resize_event : public Event {
public:
	Resize_event(unsigned old_width, unsigned old_height, unsigned new_width, unsigned new_height);
	
	unsigned old_width() const { return old_width_; }
	unsigned old_height() const { return old_height_; }
	unsigned new_width() const { return new_width_; }
	unsigned new_height() const { return new_height_; }

protected:
	unsigned old_width_;
	unsigned old_height_;
	unsigned new_width_;
	unsigned new_height_;
};

} // namespace mcurses
#endif // RESIZE_EVENT_HPP