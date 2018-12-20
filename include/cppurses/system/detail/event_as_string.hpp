#ifndef CPPURSES_SYSTEM_DETAIL_EVENT_AS_STRING_HPP
#define CPPURSES_SYSTEM_DETAIL_EVENT_AS_STRING_HPP
#include <string>

#include <cppurses/system/event.hpp>

namespace cppurses {
namespace detail {

/// Utility to help with logging, turning Event_type into a string description.
std::string event_type_as_string(Event::Type event_type);

/// Utility to help with logging, turning an Event into a string description.
inline std::string event_as_string(const Event& event) {
    return event_type_as_string(event.type());
}

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_EVENT_AS_STRING_HPP
