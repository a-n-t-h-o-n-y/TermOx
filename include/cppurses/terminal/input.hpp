#ifndef CPPURSES_TERMINAL_INPUT_HPP
#define CPPURSES_TERMINAL_INPUT_HPP
#include <optional>

#include <cppurses/system/event.hpp>

namespace cppurses::input {

/// Wait for user input, and return with a corresponding Event.
/** Blocking call, input can be received from the keyboard, mouse, or the
 *  terminal being resized. Will return nullptr if there is an error. */
auto get() -> std::optional<Event>;

}  // namespace cppurses::input
#endif  // CPPURSES_TERMINAL_INPUT_HPP
