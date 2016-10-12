#ifndef POSTED_EVENT_QUEUE_HPP
#define POSTED_EVENT_QUEUE_HPP

#include "posted_event.hpp"

#include <queue>

namespace mcurses
{
namespace detail
{

// implements priority sorting and(?) collapsing of redraw events
class Posted_event_queue {
public:
	void add_event(const Posted_event& ev);

private:
	std::queue<Posted_event> queue_;
};

} // namespace detail
} // namespace mcurses
#endif // POSTED_EVENT_QUEUE_HPP