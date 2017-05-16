#include "system_module/events/move_event.hpp"
#include "system_module/event.hpp"
#include <cstddef>

namespace twf {

Move_event::Move_event(std::size_t new_x,
                       std::size_t new_y,
                       std::size_t old_x,
                       std::size_t old_y)
    : Event{Event::Move},
      new_x_{new_x},
      new_y_{new_y},
      old_x_{old_x},
      old_y_{old_y} {}

}  // namespace twf
