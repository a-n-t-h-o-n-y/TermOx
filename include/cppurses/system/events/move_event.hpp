#ifndef SYSTEM_EVENTS_MOVE_EVENT_HPP
#define SYSTEM_EVENTS_MOVE_EVENT_HPP
#include <cppurses/system/event.hpp>

#include <cstddef>

namespace cppurses {
class Event_handler;

class Move_event : public Event {
   public:
    // In global Point.
    Move_event(Event_handler* receiver,
               std::size_t new_x,
               std::size_t new_y,
               std::size_t old_x = 0,
               std::size_t old_y = 0);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;

   protected:
    std::size_t new_x_;
    std::size_t new_y_;
    std::size_t old_x_;
    std::size_t old_y_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_MOVE_EVENT_HPP
