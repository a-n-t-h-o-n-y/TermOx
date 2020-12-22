#ifndef TERMOX_SYSTEM_DETAIL_IS_SENDABLE_HPP
#define TERMOX_SYSTEM_DETAIL_IS_SENDABLE_HPP
#include <termox/system/event.hpp>

namespace ox::detail {

/// Defaults to sending if the reciever of the event is enabled.
template <typename T>
auto is_sendable(T const& event) -> bool
{
    return event.receiver.get().is_enabled();
}

// Always Sendable
inline auto is_sendable(Child_added_event const&) -> bool { return true; }
inline auto is_sendable(Child_removed_event const&) -> bool { return true; }
inline auto is_sendable(Delete_event const&) -> bool { return true; }
inline auto is_sendable(Disable_event const&) -> bool { return true; }
inline auto is_sendable(Focus_out_event const&) -> bool { return true; }
inline auto is_sendable(Custom_event const&) -> bool { return true; }

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_IS_SENDABLE_HPP
