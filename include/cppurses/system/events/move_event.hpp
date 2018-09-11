#ifndef CPPURSES_SYSTEM_EVENTS_MOVE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_MOVE_EVENT_HPP
#include <cppurses/system/event.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
class Widget;

class Move_event : public Event {
   public:
    // In global Point.
    Move_event(Widget* receiver,
               Point new_position,
               Point old_position = Point{0, 0});

    bool send() const override;
    bool filter_send(Widget* filter) const override;

   protected:
    Point new_position_;
    mutable Point old_position_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_MOVE_EVENT_HPP
