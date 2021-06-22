#ifndef TERMOX_SYSTEM_DETAIL_FILTER_SEND_HPP
#define TERMOX_SYSTEM_DETAIL_FILTER_SEND_HPP
#include <termox/system/event_fwd.hpp>
#include "termox/system/event.hpp"

namespace ox::detail {

[[nodiscard]] auto filter_send(ox::Paint_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Key_press_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Key_release_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Mouse_press_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Mouse_release_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Mouse_wheel_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Mouse_move_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Child_added_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Child_removed_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Child_polished_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Delete_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Disable_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Enable_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Focus_in_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Focus_out_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Move_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Resize_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Timer_event const& e) -> bool;

[[nodiscard]] auto filter_send(ox::Dynamic_color_event const&) -> bool;

[[nodiscard]] auto filter_send(::esc::Window_resize) -> bool;

[[nodiscard]] auto filter_send(ox::Custom_event const& e) -> bool;

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_FILTER_SEND_HPP
