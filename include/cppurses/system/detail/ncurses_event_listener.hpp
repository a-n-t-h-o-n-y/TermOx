#ifndef NCURSES_EVENT_LISTENER_HPP
#define NCURSES_EVENT_LISTENER_HPP
#include "system/detail/abstract_event_listener.hpp"
#include <memory>

namespace cppurses {
class Widget;
class Event;
namespace detail {

class NCurses_event_listener : public Abstract_event_listener {
   public:
    std::unique_ptr<Event> get_input() const override;
    void enable_ctrl_characters() override;
    void disable_ctrl_characters() override;

   private:
    std::unique_ptr<Event> parse_mouse_event() const;

    std::unique_ptr<Event> handle_keyboard_event(int input) const;
    Widget* handle_keyboard_widget() const;
    std::unique_ptr<Event> handle_resize_event() const;
    Widget* handle_resize_widget() const;
};

}  // namespace detail
}  // namespace cppurses
#endif  // NCURSES_EVENT_LISTENER_HPP
