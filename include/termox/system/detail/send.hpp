#ifndef TERMOX_SYSTEM_DETAIL_SEND_HPP
#define TERMOX_SYSTEM_DETAIL_SEND_HPP
#include <termox/system/event_fwd.hpp>

namespace esc {
struct Window_resize;
}  // namespace esc

namespace ox::detail {

void send(ox::Paint_event e);

void send(ox::Key_press_event e);

void send(ox::Mouse_press_event e);

void send(ox::Mouse_release_event e);

void send(ox::Mouse_wheel_event e);

void send(ox::Mouse_move_event e);

void send(ox::Child_added_event e);

void send(ox::Child_removed_event e);

void send(ox::Child_polished_event e);

void send(ox::Delete_event e);

void send(ox::Disable_event e);

void send(ox::Enable_event e);

void send(ox::Focus_in_event e);

void send(ox::Focus_out_event e);

void send(ox::Move_event e);

void send(ox::Resize_event e);

void send(ox::Timer_event e);

void send(ox::Dynamic_color_event const& e);

/// Modify Screen_buffers object and post event to head Widget.
void send(::esc::Window_resize x);

void send(ox::Custom_event e);

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_SEND_HPP
