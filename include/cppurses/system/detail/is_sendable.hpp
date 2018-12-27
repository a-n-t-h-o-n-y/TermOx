#ifndef CPPURSES_SYSTEM_DETAIL_IS_SENDABLE_HPP
#define CPPURSES_SYSTEM_DETAIL_IS_SENDABLE_HPP
#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {

/// Indicates if the receiver of the Event is able to have the Event sent to it.
inline bool is_sendable(const Event& event) {
    return event.receiver().enabled() ||
           (event.type() == Event::Delete || event.type() == Event::Disable ||
            event.type() == Event::FocusOut);
}

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_IS_SENDABLE_HPP
