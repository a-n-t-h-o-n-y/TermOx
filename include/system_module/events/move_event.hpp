#ifndef MOVE_EVENT_HPP
#define MOVE_EVENT_HPP

#include "../event.hpp"

namespace twf {

class Move_event : public Event {
   public:
    Move_event(std::size_t new_x,
               std::size_t new_y,
               std::size_t old_x = 0,
               std::size_t old_y = 0);

    std::size_t new_x() const { return new_x_; }
    std::size_t new_y() const { return new_y_; }
    std::size_t old_x() const { return old_x_; }
    std::size_t old_y() const { return old_y_; }

   protected:
    std::size_t new_x_;
    std::size_t new_y_;
    std::size_t old_x_;
    std::size_t old_y_;
};

}  // namespace twf
#endif  // MOVE_EVENT_HPP
