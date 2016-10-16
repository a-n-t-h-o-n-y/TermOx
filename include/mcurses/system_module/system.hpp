#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "object.hpp"
#include "event.hpp"

#include <mcurses/signal_module/slot.hpp>
#include <mcurses/widget_module/widget.hpp>

#include <memory>

namespace mcurses
{

class System : public Object {
public:
	static void post_event(Object* obj, std::unique_ptr<Event> event, int priority = 0);
	static void remove_posted_event(Event* event);
	static bool send_event(Object* obj, const Event& event);
	static void send_posted_events(Object* obj = nullptr, Event::Type et = Event::Type::None);
	static bool notify(Object* obj, const Event& event);
	static void exit(int return_code = 0);
	static Object* head();
	static unsigned max_height;
	static unsigned max_width;

	System();
	~System();

	void set_head(Object* obj);
	int run();

	// Slots
	slot<void()> quit = [](){System::exit();};

	friend class Abstract_event_dispatcher;
private:
	static Object* head_;

};

} // namespace mcurses
#endif // SYSTEM_HPP