#ifndef CHILD_EVENT_HPP
#define CHILD_EVENT_HPP

#include "../event.hpp"

namespace mcurses {

class Child_event : public Event {
public:
	Child_event(Event::Type type):Event(type){}

};

} // namespace mcurses
#endif // CHILD_EVENT_HPP