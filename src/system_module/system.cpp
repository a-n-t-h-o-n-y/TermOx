#include <mcurses/system_module/system.hpp>
#include <mcurses/system_module/detail/thread_data.hpp>

namespace mcurses
{

int System::run()
{
	Event_loop main_loop;

	detail::Thread_data& main_thread_data = detail::Thread_data::current();
	main_thread_data.event_loops_.push(main_loop);
	main_thread_data.quit_now_ = false;

	int return_code = main_loop.run();

	main_thread_data.quit_now_ = false;

	return return_code;
}

} // namespace mcurses