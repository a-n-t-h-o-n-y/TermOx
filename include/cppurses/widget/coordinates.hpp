#ifndef WIDGET_COORDINATES_HPP
#define WIDGET_COORDINATES_HPP

#include <cstddef>

namespace cppurses {

struct Coordinates {
    std::size_t x = 0;
    std::size_t y = 0;
};

inline bool operator==(const Coordinates& left, const Coordinates& right) {
    return left.x == right.x && left.y == right.y;
}

}  // namespace cppurses
#endif  // WIDGET_COORDINATES_HPP
