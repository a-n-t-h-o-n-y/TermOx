#ifndef POSTED_EVENT_HPP
#define POSTED_EVENT_HPP

#include "../object.hpp"
#include "../event.hpp"

#include <memory>

namespace mcurses
{
namespace detail
{

class Posted_event {
public:
	Posted_event(Object* obj, std::unique_ptr<Event> ev, int priority);
	Posted_event(const Posted_event&) = delete;
	Posted_event& operator=(const Posted_event&) = delete;
	Posted_event(Posted_event&&) = default;
	Posted_event& operator=(Posted_event&&) = default;

	Object* reciever() {return reciever_;}
	Event& event() {return *event_;}
	int priority() const {return priority_;}

private:
	Object*					reciever_;
	std::unique_ptr<Event> 	event_;
	int 					priority_;
};

bool operator<(int priority, const Posted_event& pe);
bool operator<(const Posted_event& pe, int priority);
bool operator>(int priority, const Posted_event& pe);
bool operator>(const Posted_event& pe, int priority);

} // namespace detail
} // namespace mcurses
#endif // POSTED_EVENT_HPP