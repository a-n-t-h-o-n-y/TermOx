#ifndef CPPURSES_TERMINAL_INPUT_HPP
#define CPPURSES_TERMINAL_INPUT_HPP
#include <memory>

namespace cppurses {
class Event;
namespace input {

/// Wait for user input, and return with a cooresponding Event.
/** Blocking call, input can be received from the keyboard, mouse, or the
 *  terminal being resized. Will return nullptr if there is an error. */
std::unique_ptr<Event> get();

/// Signal handler used by Terminal for terminal resize signals.
void indicate_resize(int);

}  // namespace input
}  // namespace cppurses
#endif  // CPPURSES_TERMINAL_INPUT_HPP
