#include "system_module/events/resize_event.hpp"
#include <cstddef>

namespace cppurses {

Resize_event::Resize_event(std::size_t new_width,
                           std::size_t new_height,
                           std::size_t old_width,
                           std::size_t old_height)
    : Event{Event::Resize},
      new_width_{new_width},
      new_height_{new_height},
      old_width_{old_width},
      old_height_{old_height} {}

}  // namespace cppurses
