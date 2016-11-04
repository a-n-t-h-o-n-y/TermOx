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
#include <mcurses/painter_module/palette.hpp>
#include <mcurses/painter_module/color.hpp>

// #include <ncurses.h>

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

std::unique_ptr<Palette> System::system_palette_ = nullptr;

void
System::set_palette(std::unique_ptr<Palette> palette) {
	system_palette_ = std::move(palette);
	Palette* p = System::palette();

	System::paint_engine()->set_rgb(Color::Black, p->red_value(Color::Black), p->green_value(Color::Black), p->blue_value(Color::Black));
	System::paint_engine()->set_rgb(Color::Dark_red, p->red_value(Color::Dark_red), p->green_value(Color::Dark_red), p->blue_value(Color::Dark_red));
	System::paint_engine()->set_rgb(Color::Dark_blue, p->red_value(Color::Dark_blue), p->green_value(Color::Dark_blue), p->blue_value(Color::Dark_blue));
	System::paint_engine()->set_rgb(Color::Dark_gray, p->red_value(Color::Dark_gray), p->green_value(Color::Dark_gray), p->blue_value(Color::Dark_gray));
	System::paint_engine()->set_rgb(Color::Brown, p->red_value(Color::Brown), p->green_value(Color::Brown), p->blue_value(Color::Brown));
	System::paint_engine()->set_rgb(Color::Green, p->red_value(Color::Green), p->green_value(Color::Green), p->blue_value(Color::Green));
	System::paint_engine()->set_rgb(Color::Red, p->red_value(Color::Red), p->green_value(Color::Red), p->blue_value(Color::Red));
	System::paint_engine()->set_rgb(Color::Gray, p->red_value(Color::Gray), p->green_value(Color::Gray), p->blue_value(Color::Gray));
	System::paint_engine()->set_rgb(Color::Blue, p->red_value(Color::Blue), p->green_value(Color::Blue), p->blue_value(Color::Blue));
	System::paint_engine()->set_rgb(Color::Orange, p->red_value(Color::Orange), p->green_value(Color::Orange), p->blue_value(Color::Orange));
	System::paint_engine()->set_rgb(Color::Light_gray, p->red_value(Color::Light_gray), p->green_value(Color::Light_gray), p->blue_value(Color::Light_gray));
	System::paint_engine()->set_rgb(Color::Light_green, p->red_value(Color::Light_green), p->green_value(Color::Light_green), p->blue_value(Color::Light_green));
	System::paint_engine()->set_rgb(Color::Violet, p->red_value(Color::Violet), p->green_value(Color::Violet), p->blue_value(Color::Violet));
	System::paint_engine()->set_rgb(Color::Light_blue, p->red_value(Color::Light_blue), p->green_value(Color::Light_blue), p->blue_value(Color::Light_blue));
	System::paint_engine()->set_rgb(Color::Yellow, p->red_value(Color::Yellow), p->green_value(Color::Yellow), p->blue_value(Color::Yellow));
	System::paint_engine()->set_rgb(Color::White, p->red_value(Color::White), p->green_value(Color::White), p->blue_value(Color::White));
	return;
}

Palette* System::palette() {
	return system_palette_.get();
}

System::System(std::unique_ptr<Paint_engine> engine)
{
	System::set_paint_engine(std::move(engine));

	// Goes in the paint engine constructor
	// ::setlocale(LC_ALL, "en_US.UTF-8");
	// ::initscr();
	// ::cbreak(); // change to raw() once you can handle exit signals on your own.
	// ::noecho();
	// ::keypad(::stdscr, true);
	// ::mousemask(ALL_MOUSE_EVENTS, nullptr);
	// ::mouseinterval(0);
	// ::curs_set(0); // invisible cursor
	// ::start_color();

	System::set_palette(std::make_unique<DawnBringer_palette>());
}

System::~System()
{
	// ::endwin();
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