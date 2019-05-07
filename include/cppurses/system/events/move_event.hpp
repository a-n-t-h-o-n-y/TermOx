#ifndef CPPURSES_SYSTEM_EVENTS_MOVE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_MOVE_EVENT_HPP
#include <cppurses/system/event.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
class Widget;

class Move_event : public Event {
   public:
    // In global Point.
    Move_event(Widget& receiver, Point new_position)
        : Event{Event::Move, receiver}, new_position_{new_position} {}

    bool send() const override;
    bool filter_send(Widget& filter) const override;

   protected:
    Point new_position_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_MOVE_EVENT_HPP
