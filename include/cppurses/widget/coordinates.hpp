#ifndef WIDGET_COORDINATES_HPP
#define WIDGET_COORDINATES_HPP
#include <cstddef>

namespace cppurses {

struct Coordinates {
    std::size_t x = 0;
    std::size_t y = 0;
};

inline bool operator==(const Coordinates& lhs, const Coordinates& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator<(const Coordinates& lhs, const Coordinates& rhs) {
    if (lhs.y < rhs.y) {
        return true;
    } else if (lhs.y == rhs.y && lhs.x < rhs.x) {
        return true;
    }
    return false;
}

}  // namespace cppurses
#endif  // WIDGET_COORDINATES_HPP
