#include <mcurses/system_module/events/move_event.hpp>

#include <mcurses/system_module/event.hpp>

namespace mcurses {

Move_event::Move_event(unsigned new_x, unsigned new_y, unsigned old_x, unsigned old_y)
:Event{Event::Type::Move},
new_x_{new_x}, new_y_{new_y},
old_x_{old_x}, old_y_{old_y}
{}

} // namespace mcurses