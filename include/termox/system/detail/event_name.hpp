#ifndef TERMOX_SYSTEM_DETAIL_EVENT_NAME_HPP
#define TERMOX_SYSTEM_DETAIL_EVENT_NAME_HPP
#include <string>

#include <termox/system/event_fwd.hpp>

namespace ox::detail {

/// Return the name of the given Event type.
[[nodiscard]] auto name(Paint_event const&) -> std::string;

[[nodiscard]] auto name(Key_press_event const&) -> std::string;

[[nodiscard]] auto name(Mouse_press_event const&) -> std::string;

[[nodiscard]] auto name(Mouse_release_event const&) -> std::string;

[[nodiscard]] auto name(Mouse_wheel_event const&) -> std::string;

[[nodiscard]] auto name(Mouse_move_event const&) -> std::string;

[[nodiscard]] auto name(Child_added_event const&) -> std::string;

[[nodiscard]] auto name(Child_removed_event const&) -> std::string;

[[nodiscard]] auto name(Child_polished_event const&) -> std::string;

[[nodiscard]] auto name(Delete_event const&) -> std::string;

[[nodiscard]] auto name(Disable_event const&) -> std::string;

[[nodiscard]] auto name(Enable_event const&) -> std::string;

[[nodiscard]] auto name(Focus_in_event const&) -> std::string;

[[nodiscard]] auto name(Focus_out_event const&) -> std::string;

[[nodiscard]] auto name(Move_event const&) -> std::string;

[[nodiscard]] auto name(Resize_event const&) -> std::string;

[[nodiscard]] auto name(Timer_event const&) -> std::string;

[[nodiscard]] auto name(Custom_event const&) -> std::string;

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_EVENT_NAME_HPP
