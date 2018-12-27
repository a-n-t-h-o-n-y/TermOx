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
    /** Will return a nullptr if getting input failed in some way. */
    std::unique_ptr<Event> get_input() const override;

   private:
    static std::unique_ptr<Event> parse_mouse_event();
    static std::unique_ptr<Event> make_resize_event();
    static std::unique_ptr<Event> make_terminal_resize_event();
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_NCURSES_EVENT_LISTENER_HPP
