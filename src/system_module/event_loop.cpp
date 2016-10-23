#include <mcurses/system_module/event_loop.hpp>
#include <mcurses/system_module/detail/thread_data.hpp>

#include <ncurses.h> // use painter instead eventually

namespace mcurses
{

int Event_loop::run()
{
	auto& data = detail::Thread_data::current();
	data.event_loops.push(this);

	// this is a loop, no need to loop
	while(!exit_)
	{
		this->process_events();
		::refresh(); // call with static from painter?
	}

	data.event_loops.pop();

	return return_code_;
}

void Event_loop::exit(int return_code)
{
	return_code_ = return_code;
	exit_ = true;
	detail::Thread_data::current().dispatcher().interrupt();
	return;
}

bool Event_loop::process_events()
{
	detail::Thread_data& data = detail::Thread_data::current();
	return data.dispatcher().process_events();
}

} // namespace mcurses