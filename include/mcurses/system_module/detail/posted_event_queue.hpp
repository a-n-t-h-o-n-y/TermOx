#ifndef POSTED_EVENT_QUEUE_HPP
#define POSTED_EVENT_QUEUE_HPP

#include "posted_event.hpp"

#include <list>

namespace mcurses {
namespace detail {

					// Do no access through std::list pointer/ref 
class Posted_event_queue : public std::list<Posted_event> {
public:
	void add_event(Posted_event& ev);
	Posted_event next_posted_event();

private:
	using std::list<Posted_event>::push_back;
	using std::list<Posted_event>::emplace_back;
	using std::list<Posted_event>::push_front;
	using std::list<Posted_event>::emplace_front;
	using std::list<Posted_event>::emplace;
	using std::list<Posted_event>::insert;
	using std::list<Posted_event>::assign;
};

} // namespace detail
} // namespace mcurses
#endif // POSTED_EVENT_QUEUE_HPP