#ifndef CPPURSES_SYSTEM_EVENTS_RESIZE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_RESIZE_EVENT_HPP
#include <cppurses/system/event.hpp>
#include <cppurses/widget/area.hpp>

namespace cppurses {
class Widget;

class Resize_event : public Event {
   public:
    Resize_event(Widget& receiver, Area new_size)
        : Event{Event::Resize, receiver}, new_area_{new_size} {}

    bool send() const override;
    bool filter_send(Widget& filter) const override;

   protected:
    Area new_area_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_RESIZE_EVENT_HPP
