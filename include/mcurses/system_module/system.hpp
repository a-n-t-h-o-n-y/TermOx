#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "object.hpp"

namespace mcurses
{

class System : public Object {
public:

	int run();

	friend class Abstract_event_dispatcher;
private:

};

} // namespace mcurses
#endif // SYSTEM_HPP