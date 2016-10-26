#include <mcurses/system_module/events/resize_event.hpp>

namespace mcurses {

Resize_event::Resize_event(unsigned old_width, unsigned old_height, unsigned new_width, unsigned new_height)
:Event{Event::Type::Resize},
old_width_{old_width}, old_height_{old_height},
new_width_{new_width}, new_height_{new_height}
{}

} // namespace mcurses