#ifndef SYSTEM_DETAIL_EVENT_INVOKER_HPP
#define SYSTEM_DETAIL_EVENT_INVOKER_HPP
#include "system/event.hpp"

namespace cppurses {
class Widget;
namespace detail {
class Event_queue;

class Event_invoker {
   public:
    void invoke(Event_queue& queue,
                Event::Type type = Event::None,
                Widget* widget_filter = nullptr);
};

}  // namespace detail
}  // namespace cppurses
#endif  // SYSTEM_DETAIL_EVENT_INVOKER_HPP
