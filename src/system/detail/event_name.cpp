#include <termox/system/detail/event_name.hpp>

#include <string>

#include <termox/system/event.hpp>

namespace ox::detail {

auto name(Paint_event const&) -> std::string { return "Paint_event"; }

auto name(Key_press_event const&) -> std::string { return "Key_press_event"; }

auto name(Mouse_press_event const&) -> std::string
{
    return "Mouse_press_event";
}

auto name(Mouse_release_event const&) -> std::string
{
    return "Mouse_release_event";
}

auto name(Mouse_wheel_event const&) -> std::string
{
    return "Mouse_wheel_event";
}

auto name(Mouse_move_event const&) -> std::string { return "Mouse_move_event"; }

auto name(Child_added_event const&) -> std::string
{
    return "Child_added_event";
}

auto name(Child_removed_event const&) -> std::string
{
    return "Child_removed_event";
}

auto name(Child_polished_event const&) -> std::string
{
    return "Child_polished_event";
}

auto name(Delete_event const&) -> std::string { return "Delete_event"; }

auto name(Disable_event const&) -> std::string { return "Disable_event"; }

auto name(Enable_event const&) -> std::string { return "Enable_event"; }

auto name(Focus_in_event const&) -> std::string { return "Focus_in_event"; }

auto name(Focus_out_event const&) -> std::string { return "Focus_out_event"; }

auto name(Move_event const&) -> std::string { return "Move_event"; }

auto name(Resize_event const&) -> std::string { return "Resize_event"; }

auto name(Timer_event const&) -> std::string { return "Timer_event"; }

auto name(Custom_event const&) -> std::string { return "Custom_event"; }

}  // namespace ox::detail
