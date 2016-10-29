#include <mcurses/system_module/system.hpp>
#include <mcurses/system_module/detail/thread_data.hpp>
#include <mcurses/system_module/detail/posted_event.hpp>
#include <mcurses/system_module/object.hpp>
#include <mcurses/system_module/event.hpp>
#include <mcurses/system_module/event_loop.hpp>
#include <mcurses/painter_module/detail/ncurses_paint_engine.hpp>
#include <mcurses/painter_module/paint_engine.hpp>
#include <mcurses/system_module/events/paint_event.hpp>
#include <mcurses/widget_module/widget.hpp>

#include <ncurses.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

namespace mcurses
{

void
System::post_event(Object* obj, std::unique_ptr<Event> event, int priority)
{
	if(!obj) { return; }
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
System::send_event(Object* obj, Event& event)
{
	if (!obj) { return false; }
	return notify(obj, event);
}

void
System::send_posted_events(Object* obj, Event::Type et)
{
	auto& queue = detail::Thread_data::current().event_queue;
	while(!queue.empty()) {
		auto& pe = queue.front();
		if ((obj == nullptr || obj == pe.reciever())
			&& (et == Event::Type::None || et == pe.event().type())
			&& ((pe.event().type() != Event::Type::DeferredDelete)
				|| et == Event::Type::DeferredDelete))
		{
			detail::Posted_event posted = queue.next_posted_event();
			if (posted.event().type() == Event::Type::DeferredDelete) {
				Object* parent = obj->parent();
				if(!parent) {
					if(parent == System::head()) {
						System::set_head(nullptr);
					} else { return; }
				}
				parent->delete_child(obj);
			} else {
				notify(posted.reciever(), posted.event());
			}
		}
	}
	return;
}

bool
System::notify(Object* obj, Event& event)
{
	bool handled {false};

	// Send event to any filter objects
	unsigned i{0};
	while (i < obj->event_filter_objects_.size() && !handled) { // While loop incase event_filter invalidates iterators
		handled = obj->event_filter_objects_[i]->event_filter(obj, event);
		++i;
	}
	if(handled) {
		return true;
	}

	return notify_helper(obj, event);
}

bool System::notify_helper(Object* obj, Event& event)
{
	bool handled {false};
	// Send event to object
	handled = obj->event(event);

	// Propagate event to parent
	if (!handled) { // && event type can propogate
		Object* parent = obj->parent();
		if (parent) {
			handled = notify_helper(parent, event);
		}
	}
	return handled;
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

Paint_engine* System::paint_engine()
{
	return engine_.get();
}

void System::set_paint_engine(std::unique_ptr<Paint_engine> engine)
{
	if(!engine) { return; }
	engine_ = std::move(engine);
	System::post_event(System::head(), std::make_unique<Paint_event>());
	return;
}

unsigned
System::max_width()
{
	return System::paint_engine()->screen_width();
}

unsigned
System::max_height()
{
	return System::paint_engine()->screen_height();
}

Object* System::head_ = nullptr;
std::unique_ptr<Paint_engine> System::engine_ = nullptr;

Object* System::head()
{
	return head_;
}

Widget* System::focus_widg_ = nullptr;

Widget* System::focus_widget()
{
	return focus_widg_;
}

void System::set_focus_widget(Widget* widg)
{
	if(focus_widg_) {
		focus_widg_->clear_focus();
	}
	focus_widg_ = widg;
	focus_widg_->set_focus(true);
	return;
}

void System::cycle_tab_focus()
{
	if(!System::head()) { return; }
	std::vector<Object*> objects;

	// Populate objects vector
	objects.push_back(System::head());
	int i{0};
	while(i < objects.size()) {
		Object* current = objects[i];
		auto children = current->children();
		std::for_each(std::begin(children), std::end(children), [&objects](Object* p){objects.push_back(p);});
		++i;
	}

	// Rearrange objects vector
	Object* current_focus_object = static_cast<Object*>(System::focus_widget());
	if(current_focus_object) {
		auto current_iter = std::find(std::begin(objects), std::end(objects), current_focus_object);
		if(current_iter != std::end(objects)) {
			std::move(std::begin(objects), current_iter+1, std::back_inserter(objects));
		}
	}

	// Find the next focus widget
	for(Object* child : objects) {
		Widget* widg = dynamic_cast<Widget*>(child);
		if(widg) {
			if(widg->focus_policy() == Widget::Focus_policy::TabFocus
			|| widg->focus_policy() == Widget::Focus_policy::StrongFocus) {
				System::set_focus_widget(widg);
			return;
			}
		}
	}
	return;	
}



System::System(std::unique_ptr<Paint_engine> engine)
{
	System::set_paint_engine(std::move(engine));

	// Theses should be passed of to a single static Painter function(s) that generalize for other systems, not just ncurses.
	// Painter::initialize();
	::setlocale(LC_ALL, "en_US.UTF-8");
	::initscr();
	::cbreak(); // change to raw() once you can handle exit signals on your own.
	::noecho();
	::keypad(::stdscr, true);
	::mousemask(ALL_MOUSE_EVENTS, nullptr);
	::mouseinterval(0);
	::curs_set(0); // invisible cursor
	::start_color();
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