#ifndef CPPURSES_SYSTEM_EVENTS_PAINT_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_PAINT_EVENT_HPP
#include <cppurses/painter/detail/is_paintable.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
class Widget;

class Paint_event : public Event {
   public:
    explicit Paint_event(Widget& receiver) : Event{Event::Paint, receiver} {}

    bool send() const override {
        return !detail::is_paintable(receiver_) ? false
                                                : receiver_.paint_event();
    }
    bool filter_send(Widget& filter) const override {
        return filter.paint_event_filter(receiver_);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_PAINT_EVENT_HPP
