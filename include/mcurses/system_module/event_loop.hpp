#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "object.hpp"

namespace mcurses
{

class Event_loop : public Object {
public:
	int run();
	bool process_events();

private:
	
};

} // namespace mcurses
#endif // EVENT_LOOP_HPP