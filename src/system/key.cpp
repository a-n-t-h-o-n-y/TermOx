#include <cppurses/system/key.hpp>

#include <cstdint>

namespace cppurses {

char key_to_char(Key key) {
    const std::int16_t alpha_low = 32;
    const std::int16_t alpha_high = 126;
    auto value = static_cast<std::int16_t>(key);
    if (value < alpha_low || value > alpha_high) {
        return '\0';
    }
    return static_cast<char>(value);
}

}  // namespace cppurses
