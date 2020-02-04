#ifndef CPPURSES_SYSTEM_EVENTS_PAINT_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_PAINT_EVENT_HPP
#include <cppurses/painter/detail/is_paintable.hpp>
#include <cppurses/system/event.hpp>

namespace cppurses {
class Widget;

class Paint_event : public Event {
   public:
    explicit Paint_event(Widget& receiver) : Event{Event::Paint, receiver} {}

    auto send() const -> bool override
    {
        return !detail::is_paintable(receiver_) ? false
                                                : receiver_.paint_event();
    }

    auto filter_send(Widget& filter) const -> bool override
    {
        return filter.paint_event_filter(receiver_);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_PAINT_EVENT_HPP
