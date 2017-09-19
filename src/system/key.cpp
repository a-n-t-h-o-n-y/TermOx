#include "system/key.hpp"

namespace cppurses {

char key_to_char(Key key) {
    const short alpha_low = 32;
    const short alpha_high = 126;
    auto value = static_cast<short>(key);
    if (value < alpha_low || value > alpha_high) {
        return '\0';
    }
    return static_cast<char>(value);
}

}  // namespace cppurses
