#ifndef INPUT_EVENT_HPP
#define INPUT_EVENT_HPP

#include "../event.hpp"

namespace mcurses {

class Input_event : public Event {
public:
	enum class KeyboardModifiers {};

protected:
	Input_event(Event::Type type):Event{type}{}
	KeyboardModifiers modifiers_;
};

} // namespace mcurses
#endif // INPUT_EVENT_HPP
