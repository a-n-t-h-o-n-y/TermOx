#ifndef CPPURSES_TERMINAL_TERMINAL_ERROR_HPP
#define CPPURSES_TERMINAL_TERMINAL_ERROR_HPP
#include <stdexcept>

namespace cppurses {

/// Runtime Terminal Error
/** Used when a terminal does not support some feature. */
struct Terminal_error : std::runtime_error {
    using std::runtime_error::runtime_error;
};

}  // namespace cppurses
#endif  // CPPURSES_TERMINAL_TERMINAL_ERROR_HPP
