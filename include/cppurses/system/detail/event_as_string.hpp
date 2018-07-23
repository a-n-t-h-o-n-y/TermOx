#ifndef CPPURSES_SYSTEM_DETAIL_EVENT_AS_STRING_HPP
#define CPPURSES_SYSTEM_DETAIL_EVENT_AS_STRING_HPP
#include <string>

namespace cppurses {
class Event;
namespace detail {

/// Utility to help with logging.
std::string event_as_string(const Event& event);

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_EVENT_AS_STRING_HPP
