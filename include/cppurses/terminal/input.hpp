#ifndef CPPURSES_TERMINAL_INPUT_HPP
#define CPPURSES_TERMINAL_INPUT_HPP
#include <memory>

namespace cppurses {
class Event;
namespace input {

/// Wait for user input, and return with a corresponding Event.
/** Blocking call, input can be received from the keyboard, mouse, or the
 *  terminal being resized. Will return nullptr if there is an error. */
auto get() -> std::unique_ptr<Event>;

}  // namespace input
}  // namespace cppurses
#endif  // CPPURSES_TERMINAL_INPUT_HPP
