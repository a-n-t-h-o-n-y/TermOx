#include <mcurses/system_module/system.hpp>
#include <mcurses/system_module/detail/thread_data.hpp>
#include <mcurses/system_module/object.hpp>
#include <mcurses/system_module/event.hpp>

namespace mcurses
{

void
System::post_event(Object* obj, std::unique_ptr<Event> event, int priority)
{
	detail::Posted_event pe(obj, std::move(event), priority);
	detail::Thread_data::current().event_queue.add_event(pe);
	return;
}

int
System::run()
{
	Event_loop main_loop;

	detail::Thread_data& data = detail::Thread_data::current();
	data.event_loops.push(&main_loop);
	data.quit_now = false;

	int return_code = main_loop.run();

	data.quit_now = false;

	return return_code;
}

} // namespace mcurses