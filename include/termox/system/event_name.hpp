#ifndef TERMOX_SYSTEM_EVENT_NAME_HPP
#define TERMOX_SYSTEM_EVENT_NAME_HPP
#include <string>

#include <termox/system/event.hpp>

namespace ox {

/// Return the name of the given Event type.
[[nodiscard]] inline auto name(Paint_event const&) -> std::string
{
    return "Paint_event";
}

[[nodiscard]] inline auto name(Key_press_event const&) -> std::string
{
    return "Key_press_event";
}

[[nodiscard]] inline auto name(Mouse_press_event const&) -> std::string
{
    return "Mouse_press_event";
}

[[nodiscard]] inline auto name(Mouse_release_event const&) -> std::string
{
    return "Mouse_release_event";
}

[[nodiscard]] inline auto name(Mouse_double_click_event const&) -> std::string
{
    return "Mouse_double_click_event";
}

[[nodiscard]] inline auto name(Mouse_wheel_event const&) -> std::string
{
    return "Mouse_wheel_event";
}

[[nodiscard]] inline auto name(Mouse_move_event const&) -> std::string
{
    return "Mouse_move_event";
}

[[nodiscard]] inline auto name(Child_added_event const&) -> std::string
{
    return "Child_added_event";
}

[[nodiscard]] inline auto name(Child_removed_event const&) -> std::string
{
    return "Child_removed_event";
}

[[nodiscard]] inline auto name(Child_polished_event const&) -> std::string
{
    return "Child_polished_event";
}

[[nodiscard]] inline auto name(Delete_event const&) -> std::string
{
    return "Delete_event";
}

[[nodiscard]] inline auto name(Disable_event const&) -> std::string
{
    return "Disable_event";
}

[[nodiscard]] inline auto name(Enable_event const&) -> std::string
{
    return "Enable_event";
}

[[nodiscard]] inline auto name(Focus_in_event const&) -> std::string
{
    return "Focus_in_event";
}

[[nodiscard]] inline auto name(Focus_out_event const&) -> std::string
{
    return "Focus_out_event";
}

[[nodiscard]] inline auto name(Move_event const&) -> std::string
{
    return "Move_event";
}

[[nodiscard]] inline auto name(Resize_event const&) -> std::string
{
    return "Resize_event";
}

[[nodiscard]] inline auto name(Timer_event const&) -> std::string
{
    return "Timer_event";
}

[[nodiscard]] inline auto name(Custom_event const&) -> std::string
{
    return "Custom_event";
}

}  // namespace ox
#endif  // TERMOX_SYSTEM_EVENT_NAME_HPP
