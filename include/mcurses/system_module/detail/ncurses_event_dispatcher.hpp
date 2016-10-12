#ifndef NCURSES_EVENT_DISPATCHER_HPP
#define NCURSES_EVENT_DISPATCHER_HPP

#include "../abstract_event_dispatcher.hpp"

namespace mcurses
{

class NCurses_event_dispatcher : public Abstract_event_dispatcher {
public:
	virtual bool process_events() override;

private:
	
};

} // namespace mcurses
#endif // NCURSES_EVENT_DISPATCHER_HPP