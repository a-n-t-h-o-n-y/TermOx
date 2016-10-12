#include <mcurses/system_module/event_loop.hpp>
#include <mcurses/system_module/detail/thread_data.hpp>

namespace mcurses
{

int Event_loop::run()
{
	if(this->process_events())
		return 0;
	else
		return 1;
}

bool Event_loop::process_events()
{
	detail::Thread_data& main_thread_data = detail::Thread_data::current();
	return main_thread_data.dispatcher().process_events();
}

} // namespace mcurses