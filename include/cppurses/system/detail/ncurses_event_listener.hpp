#ifndef NCURSES_EVENT_LISTENER_HPP
#define NCURSES_EVENT_LISTENER_HPP
#include "system/detail/abstract_event_listener.hpp"
#include <memory>

namespace cppurses {
class Object;
namespace detail {

class NCurses_event_listener : public Abstract_event_listener {
   public:
    std::unique_ptr<Event> get_input() const override;

   private:
    std::unique_ptr<Event> parse_mouse_event() const;

    std::unique_ptr<Event> handle_keyboard_event(int input) const;
    Object* handle_keyboard_object() const;
    std::unique_ptr<Event> handle_resize_event() const;
    Object* handle_resize_object() const;
};

}  // namespace detail
}  // namespace cppurses
#endif  // NCURSES_EVENT_LISTENER_HPP
