#ifndef CPPURSES_SYSTEM_DETAIL_EVENT_AS_STRING_HPP
#define CPPURSES_SYSTEM_DETAIL_EVENT_AS_STRING_HPP
#include <string>

#include <cppurses/system/event.hpp>

namespace cppurses {
namespace detail {

/// Utility to help with logging. Turns an event into a string description.
std::string event_as_string(const Event& event);

/// Utility to help with logging. Turns an event type into a string description.
std::string event_type_as_string(Event::Type event_type);

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_EVENT_AS_STRING_HPP
