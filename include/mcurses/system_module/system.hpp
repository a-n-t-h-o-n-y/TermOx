#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "object.hpp"
#include "event.hpp"

#include <mcurses/signal_module/slot.hpp>
#include <mcurses/painter_module/paint_engine.hpp>
#include <mcurses/painter_module/detail/ncurses_paint_engine.hpp>
#include <mcurses/painter_module/palette.hpp>

#include <memory>

namespace mcurses
{

class Widget;

class System : public Object {
public:
	static void post_event(Object* obj, std::unique_ptr<Event> event, int priority = 0);
	static void remove_posted_event(Event* event);
	static bool send_event(Object* obj, Event& event);
	static void send_posted_events(Object* obj = nullptr, Event::Type et = Event::Type::None);
	static bool notify(Object* obj, Event& event);
	static void exit(int return_code = 0);
	static Object* head();
	static unsigned max_width();
	static unsigned max_height();
	static Paint_engine* paint_engine();
	static void set_paint_engine(std::unique_ptr<Paint_engine> engine);
	static Widget* focus_widget();
	static void set_focus_widget(Widget* widg);
	static void cycle_tab_focus();
	static void set_palette(std::unique_ptr<Palette> palette);
	static Palette* palette();

	System(std::unique_ptr<Paint_engine> engine = std::make_unique<detail::NCurses_paint_engine>());
	~System();

	static void set_head(Object* obj);
	int run();

	// Slots
	slot<void()> quit = [](){System::exit();};

	friend class Abstract_event_dispatcher;
	
private:
	static Object* head_;
	static std::unique_ptr<Paint_engine> engine_;
	static Widget* focus_widg_;
	static std::unique_ptr<Palette> system_palette_;
	static bool notify_helper(Object* obj, Event& event);

};

} // namespace mcurses
#endif // SYSTEM_HPP
