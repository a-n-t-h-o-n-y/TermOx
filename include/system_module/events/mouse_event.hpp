#ifndef MOUSE_EVENT_HPP
#define MOUSE_EVENT_HPP

#include "input_event.hpp"

namespace mcurses {

class Mouse_event : public Input_event {
public:
	enum class Button {
		NoButton,
		LeftButton,	// BUTTON1
		MidButton,
		RightButton,
		ScrollUp,
		ScrollDown
		// ... add ?
	};

	Mouse_event(Event::Type type, Button button, unsigned glob_x, unsigned glob_y, unsigned local_x, unsigned local_y, short device_id)
	:Input_event{type}, button_{button}, glob_x_{glob_x}, glob_y_{glob_y}, local_x_{local_x}, local_y_{local_y}, device_id_{device_id}
	{}

	Button button() const { return button_; }
	unsigned global_x() const { return glob_x_; }
	unsigned global_y() const { return glob_y_; }
	unsigned local_x() const { return local_x_; }
	unsigned local_y() const { return local_y_; }
	short device_id() const { return device_id_; }

private:
	Button button_;
	unsigned glob_x_;
	unsigned glob_y_;
	unsigned local_x_;
	unsigned local_y_;
	short device_id_;
};

} // namespace mcurses
#endif // MOUSE_EVENT_HPP
