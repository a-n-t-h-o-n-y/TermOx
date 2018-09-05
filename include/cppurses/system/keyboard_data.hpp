#ifndef CPPURSES_SYSTEM_KEYBOARD_DATA_HPP
#define CPPURSES_SYSTEM_KEYBOARD_DATA_HPP
#include <cppurses/system/key.hpp>

namespace cppurses {

/// Holds data from a Key Input Event.
struct Keyboard_data {
    Key key;
    char symbol;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_KEYBOARD_DATA_HPP
