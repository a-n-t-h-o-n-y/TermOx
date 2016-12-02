#ifndef SHOW_EVENT_HPP
#define SHOW_EVENT_HPP

#include "../event.hpp"

namespace mcurses {

class Show_event : public Event {
public:
	Show_event():Event{Event::Type::Show}{}

};

} // namespace mcurses
#endif // SHOW_EVENT_HPP