#ifndef CPPURSES_SYSTEM_DETAIL_EVENT_INVOKER_HPP
#define CPPURSES_SYSTEM_DETAIL_EVENT_INVOKER_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Event_handler;
namespace detail {
class Event_queue;

/// Responsible for processing Events on a given Event_queue.
/** If \p type_filter is not Event::None, then only the given Event::Type will
 *  be invoked. If \p object_filter is not nullptr, only Events with a receiver
 *  of that \p object_filter will be invoked. Events are processed by calling
 *  System::send_event() for each Event. Allows for more Events to be appended
 *  to the Event_queue as Events are processed. Once an Event has been invoked,
 *  it is erased from the \p queue. */
class Event_invoker {
   public:
    void invoke(Event_queue& queue,
                Event::Type type_filter = Event::None,
                Event_handler* object_filter = nullptr);
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_EVENT_INVOKER_HPP
