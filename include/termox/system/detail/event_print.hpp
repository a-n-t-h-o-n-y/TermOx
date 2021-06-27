#ifndef TERMOX_SYSTEM_DETAIL_EVENT_PRINT_HPP
#define TERMOX_SYSTEM_DETAIL_EVENT_PRINT_HPP
#include <iosfwd>

#include <termox/system/event_fwd.hpp>
#include "termox/system/event.hpp"

namespace ox::detail {

void event_print(std::ostream& os, ox::Paint_event const& e);

void event_print(std::ostream& os, ox::Key_press_event const& e);

void event_print(std::ostream& os, ox::Mouse_press_event const& e);

void event_print(std::ostream& os, ox::Mouse_release_event const& e);

void event_print(std::ostream& os, ox::Mouse_wheel_event const& e);

void event_print(std::ostream& os, ox::Mouse_move_event const& e);

void event_print(std::ostream& os, ox::Child_added_event const& e);

void event_print(std::ostream& os, ox::Child_removed_event const& e);

void event_print(std::ostream& os, ox::Child_polished_event const& e);

void event_print(std::ostream& os, ox::Delete_event const& e);

void event_print(std::ostream& os, ox::Disable_event const& e);

void event_print(std::ostream& os, ox::Enable_event const& e);

void event_print(std::ostream& os, ox::Focus_in_event const& e);

void event_print(std::ostream& os, ox::Focus_out_event const& e);

void event_print(std::ostream& os, ox::Move_event const& e);

void event_print(std::ostream& os, ox::Resize_event const& e);

void event_print(std::ostream& os, ox::Timer_event const& e);

void event_print(std::ostream& os, ox::Custom_event const&);

void event_print(std::ostream& os, ox::Dynamic_color_event const&);

void event_print(std::ostream& os, ::esc::Window_resize const&);

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_EVENT_PRINT_HPP
