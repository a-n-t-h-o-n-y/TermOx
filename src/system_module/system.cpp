#include <mcurses/system_module/system.hpp>
#include <mcurses/system_module/detail/thread_data.hpp>
#include <mcurses/system_module/detail/posted_event.hpp>
#include <mcurses/system_module/object.hpp>
#include <mcurses/system_module/event.hpp>
#include <mcurses/system_module/event_loop.hpp>

#include <ncurses.h>

#include <algorithm>

namespace mcurses
{

void
System::post_event(Object* obj, std::unique_ptr<Event> event, int priority)
{
	detail::Posted_event pe(obj, std::move(event), priority);
	detail::Thread_data::current().event_queue.add_event(pe);
	return;
}

void
System::remove_posted_event(Event* event)
{
	mcurses::detail::Posted_event_queue& queue = detail::Thread_data::current().event_queue;
	mcurses::detail::Posted_event_queue::iterator pos = std::find_if(std::begin(queue), std::end(queue), [&event](auto& pe_){return (&(pe_.event()) == event);});
	if(pos == std::end(queue)) {
		return;
	}
	else {
		queue.erase(pos);
	}
	return;
}

bool
System::send_event(Object* obj, const Event& event)
{
	return notify(obj, event);
}

void
System::send_posted_events(Object* obj, Event::Type et)
{
	std::vector<Event*> to_remove;
	auto& queue = detail::Thread_data::current().event_queue;
	for(detail::Posted_event& pe : queue)
	{
		if((obj == nullptr || obj == pe.reciever())
			&& (et == Event::Type::None || et == pe.event().type())
			&& ((pe.event().type() != Event::Type::DeferredDelete)
				|| et == Event::Type::DeferredDelete))
		{
			notify(pe.reciever(), pe.event());
			to_remove.push_back(&pe.event());
		}
	}

	// Remove events all at once so as to not invalidate iterators during above for range loop
	for(Event* ep : to_remove) {
		remove_posted_event(ep);
	}
	return;
}

bool
System::notify(Object* obj, const Event& event)
{
	// propogates mouse and others up to parents if returns false
	bool ret = obj->event(event);
	if(ret == false) { // && event type is mouse or key
		ret = notify(obj->parent(), event);
	}

	return ret;
}

void System::exit(int return_code)
{
	auto& data = detail::Thread_data::current();
	data.quit_now = true;
	// call exit on each event loop
	for(int i{0}; i<data.event_loops.size(); ++i)
	{
		data.event_loops.top()->exit(return_code);
		data.event_loops.pop();
	}

	return;
}

Object* System::head_ = nullptr;

Object* System::head()
{
	return head_;
}

unsigned System::max_width = 0;
unsigned System::max_height = 0;

System::System()
{
	// Theses should be passed of to a single static Painter function(s) that generalize for other systems, not just ncurses.
	// Painter::initialize();
	::initscr();
	::cbreak(); // change to raw() once you can handle exit signals on your own.
	::noecho();
	::keypad(::stdscr, true);
	::mousemask(ALL_MOUSE_EVENTS, nullptr);
	::mouseinterval(0);

	// now initialize max height and width
	System::max_width = ::COLS;
	System::max_height = ::LINES;
}

System::~System()
{
	::endwin();
}

void
System::set_head(Object* obj)
{
	head_ = obj;
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