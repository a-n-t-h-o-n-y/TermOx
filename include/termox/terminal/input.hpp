#ifndef TERMOX_TERMINAL_INPUT_HPP
#define TERMOX_TERMINAL_INPUT_HPP
#include <optional>

#include <termox/system/event.hpp>

namespace ox::input {

/// Wait for user input, and return with a corresponding Event.
/** Blocking call, input can be received from the keyboard, mouse, or the
 *  terminal being resized. Will return nullptr if there is an error. */
auto get() -> std::optional<Event>;

}  // namespace ox::input
#endif  // TERMOX_TERMINAL_INPUT_HPP
