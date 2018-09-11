#ifndef CPPURSES_SYSTEM_KEYBOARD_DATA_HPP
#define CPPURSES_SYSTEM_KEYBOARD_DATA_HPP
#include <cppurses/system/key.hpp>

namespace cppurses {

/// Holds data from a Key Input Event.
struct Keyboard_data {
    /// The keycode of the input.
    Key key;

    /// The symbol representing the keycode of the input.
    /** Is '\0' if keycode is not printable. */
    char symbol;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_KEYBOARD_DATA_HPP
