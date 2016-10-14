#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "object.hpp"
#include "event.hpp"

#include <memory>

namespace mcurses
{

class System : public Object {
public:

	static void post_event(Object* obj, std::unique_ptr<Event> event, int priority = 0);

	int run();

	friend class Abstract_event_dispatcher;
private:

};

} // namespace mcurses
#endif // SYSTEM_HPP