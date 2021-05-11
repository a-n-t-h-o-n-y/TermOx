#include <termox/system/detail/is_sendable.hpp>

#include <termox/system/event.hpp>
#include <termox/widget/widget.hpp>

namespace ox::detail {

auto is_sendable(ox::Key_press_event const& event) -> bool
{
    if (event.receiver.has_value())
        return event.receiver->get().is_enabled();
    return false;
}

// Below are always Sendable.
[[nodiscard]] auto is_sendable(ox::Child_added_event const&) -> bool
{
    return true;
}

[[nodiscard]] auto is_sendable(ox::Child_removed_event const&) -> bool
{
    return true;
}

[[nodiscard]] auto is_sendable(ox::Child_polished_event const&) -> bool
{
    return true;
}

[[nodiscard]] auto is_sendable(ox::Delete_event const&) -> bool { return true; }

[[nodiscard]] auto is_sendable(ox::Disable_event const&) -> bool
{
    return true;
}

[[nodiscard]] auto is_sendable(ox::Focus_out_event const&) -> bool
{
    return true;
}

[[nodiscard]] auto is_sendable(ox::Custom_event const&) -> bool { return true; }

[[nodiscard]] auto is_sendable(ox::Dynamic_color_event const&) -> bool
{
    return true;
}

[[nodiscard]] auto is_sendable(::esc::Window_resize const&) -> bool
{
    return true;
}

}  // namespace ox::detail
