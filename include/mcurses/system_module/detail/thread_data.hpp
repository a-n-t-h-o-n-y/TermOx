#ifndef THREAD_DATA_HPP
#define THREAD_DATA_HPP

#include "../abstract_event_dispatcher.hpp"
#include "ncurses_event_dispatcher.hpp"
#include "../event_loop.hpp"
#include "posted_event_queue.hpp"

#include <stack>
#include <memory>

namespace mcurses
{
namespace detail
{

class Thread_data {
public:
	static Thread_data& current();

	Abstract_event_dispatcher& dispatcher();

	bool 						quit_now_ = false;
	std::stack<Event_loop>		event_loops_;
	Posted_event_queue			event_queue_;

private:
	std::unique_ptr<Abstract_event_dispatcher> 	dispatcher_ = std::make_unique<NCurses_event_dispatcher>();
	Thread_data(){}
};

} // namespace detail
} // namespace mcurses
#endif // THREAD_DATA_HPP