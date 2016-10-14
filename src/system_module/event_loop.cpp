#include <mcurses/system_module/event_loop.hpp>
#include <mcurses/system_module/detail/thread_data.hpp>

namespace mcurses
{

int Event_loop::run()
{
	auto& data = detail::Thread_data::current();
	data.event_loops.push(this);

	// this is a loop, no need to loop
	this->process_events();

	data.event_loops.pop();

	return 0;
}

bool Event_loop::process_events()
{
	detail::Thread_data& data = detail::Thread_data::current();
	return data.dispatcher().process_events();	// this loops
}

} // namespace mcurses