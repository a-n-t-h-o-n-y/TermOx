#ifndef CPPURSES_NCURSES_EVENT_LISTENER_HPP
#define CPPURSES_NCURSES_EVENT_LISTENER_HPP
#include <memory>

#include <cppurses/system/detail/abstract_event_listener.hpp>

namespace cppurses {
class Widget;
class Event;
namespace detail {

/// Responsible for retrieving Event objects from user input, using NCurses.
/** Retrieves user input from NCurses and returns Event objects. Handles mouse,
 *  keyboard, and terminal resize inputs.*/
class NCurses_event_listener : public Abstract_event_listener {
   public:
    /// Blocks on NCurses getch(), parses input and returns an Event object.
    std::unique_ptr<Event> get_input() const override;

   private:
    std::unique_ptr<Event> parse_mouse_event() const;

    std::unique_ptr<Event> handle_keyboard_event(Widget& receiver,
                                                 int input) const;
    Widget* handle_keyboard_widget() const;
    std::unique_ptr<Event> handle_resize_event(Widget& receiver) const;
    Widget* handle_resize_widget() const;
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_NCURSES_EVENT_LISTENER_HPP
