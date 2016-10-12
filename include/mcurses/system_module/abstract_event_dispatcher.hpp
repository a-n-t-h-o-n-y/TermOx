#ifndef ABSTRACT_EVENT_DISPATCHER_HPP
#define ABSTRACT_EVENT_DISPATCHER_HPP

#include "object.hpp"

namespace mcurses
{

class Abstract_event_dispatcher : public Object {
public:
	virtual bool process_events() = 0;

private:
	
};

} // namespace mcurses
#endif // ABSTRACT_EVENT_DISPATCHER_HPP