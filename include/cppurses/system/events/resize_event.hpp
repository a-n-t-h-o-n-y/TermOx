#ifndef CPPURSES_SYSTEM_EVENTS_RESIZE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_RESIZE_EVENT_HPP
#include <cppurses/system/event.hpp>
#include <cppurses/widget/area.hpp>

namespace cppurses {
class Widget;

class Resize_event : public Event {
   public:
    Resize_event(Widget& receiver, Area new_size, Area old_size = Area{0, 0})
    : Event{Event::Resize, receiver},
      new_size_{new_size},
      old_size_{old_size} {}

    bool send() const override;
    bool filter_send(Widget& filter) const override;

   protected:
    Area new_size_;
    mutable Area old_size_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_RESIZE_EVENT_HPP
