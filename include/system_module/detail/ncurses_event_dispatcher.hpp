#ifndef NCURSES_EVENT_DISPATCHER_HPP
#define NCURSES_EVENT_DISPATCHER_HPP

#include "../abstract_event_dispatcher.hpp"
#include "../event.hpp"
#include "../object.hpp"

#include <memory>

namespace mcurses {
namespace detail {

class NCurses_event_dispatcher : public Abstract_event_dispatcher {
public:

protected:
	void post_user_input() override;
	std::pair<Object*, std::unique_ptr<Event>> parse_mouse_event();
	Object* find_object(unsigned x, unsigned y);
	std::unique_ptr<Event> handle_keyboard_event(int input);
	Object* handle_keyboard_object();
	std::unique_ptr<Event> handle_resize_event();
	Object* handle_resize_object();
};

} // namespace detail
} // namespace mcurses
#endif // NCURSES_EVENT_DISPATCHER_HPP