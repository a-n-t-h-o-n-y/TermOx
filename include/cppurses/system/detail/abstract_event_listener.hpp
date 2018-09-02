#ifndef CPPURSES_SYSTEM_DETAIL_ABSTRACT_EVENT_LISTENER_HPP
#define CPPURSES_SYSTEM_DETAIL_ABSTRACT_EVENT_LISTENER_HPP
#include <memory>

namespace cppurses {
class Event;
namespace detail {

/// Interface for event listeners.
/** Event_listeners are responsible for acquiring user input and parsing it into
 *  an Event. Can be overridden for different input sources. */
class Abstract_event_listener {
   public:
    virtual ~Abstract_event_listener() = default;

    /// Responsible for aquiring an input from the os, and returning an Event.
    virtual std::unique_ptr<Event> get_input() const = 0;
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_ABSTRACT_EVENT_LISTENER_HPP
