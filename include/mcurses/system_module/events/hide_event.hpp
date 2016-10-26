#ifndef HIDE_EVENT_HPP
#define HIDE_EVENT_HPP

#include "../event.hpp"

namespace mcurses {

class Hide_event : public Event {
public:
	Hide_event():Event{Event::Type::Hide}{}

};

} // namespace mcurses
#endif // HIDE_EVENT_HPP