#ifndef TERMOX_SYSTEM_DETAIL_IS_SENDABLE_HPP
#define TERMOX_SYSTEM_DETAIL_IS_SENDABLE_HPP
#include <termox/system/event_fwd.hpp>

namespace esc {
struct Window_resize;
}  // namespace esc

namespace ox::detail {

/// Defaults to sending if the reciever of the event is enabled.
template <typename T>
[[nodiscard]] auto is_sendable(T const& event) -> bool
{
    return event.receiver.get().is_enabled();
}

[[nodiscard]] auto is_sendable(ox::Move_event const& event) -> bool;

[[nodiscard]] auto is_sendable(ox::Resize_event const& event) -> bool;

[[nodiscard]] auto is_sendable(ox::Key_press_event const& event) -> bool;

[[nodiscard]] auto is_sendable(ox::Child_added_event const&) -> bool;

[[nodiscard]] auto is_sendable(ox::Child_removed_event const&) -> bool;

[[nodiscard]] auto is_sendable(ox::Child_polished_event const&) -> bool;

[[nodiscard]] auto is_sendable(ox::Delete_event const&) -> bool;

[[nodiscard]] auto is_sendable(ox::Disable_event const&) -> bool;

[[nodiscard]] auto is_sendable(ox::Focus_in_event const&) -> bool;

[[nodiscard]] auto is_sendable(ox::Focus_out_event const&) -> bool;

[[nodiscard]] auto is_sendable(ox::Custom_event const&) -> bool;

[[nodiscard]] auto is_sendable(ox::Dynamic_color_event const&) -> bool;

[[nodiscard]] auto is_sendable(::esc::Window_resize const&) -> bool;

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_IS_SENDABLE_HPP
