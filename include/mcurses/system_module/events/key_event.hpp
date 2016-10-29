#ifndef KEY_EVENT_HPP
#define KEY_EVENT_HPP

#include "../event.hpp"

namespace mcurses {

class Key_event : public Event {
public:
	Key_event(Event::Type type, int key_code):Event{type}, key_code_{key_code}{}
	int key() const { return key_code_; }
	char text() const { return key_code_; }

private:
	int key_code_;
};

} // namespace mcurses
#endif // KEY_EVENT_HPP