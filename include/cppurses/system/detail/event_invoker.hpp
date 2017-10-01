#ifndef SYSTEM_DETAIL_EVENT_INVOKER_HPP
#define SYSTEM_DETAIL_EVENT_INVOKER_HPP
#include "system/event.hpp"

namespace cppurses {
class Event_handler;
namespace detail {
class Event_queue;

class Event_invoker {
   public:
    void invoke(Event_queue& queue,
                Event::Type type_filter = Event::None,
                Event_handler* object_filter = nullptr);
};

}  // namespace detail
}  // namespace cppurses
#endif  // SYSTEM_DETAIL_EVENT_INVOKER_HPP
