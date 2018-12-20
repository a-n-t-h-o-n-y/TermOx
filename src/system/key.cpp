#include <cppurses/system/key.hpp>

#include <cstdint>

namespace cppurses {

char key_to_char(Key key) {
    const auto alpha_low = std::int16_t{32};
    const auto alpha_high = std::int16_t{126};
    const auto value = static_cast<std::int16_t>(key);
    if (value < alpha_low || value > alpha_high) {
        return '\0';
    }
    return static_cast<char>(value);
}

}  // namespace cppurses
