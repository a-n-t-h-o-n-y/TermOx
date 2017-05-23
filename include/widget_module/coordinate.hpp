#ifndef WIDGET_MODULE_COORDINATE_HPP
#define WIDGET_MODULE_COORDINATE_HPP

#include <cstddef>

namespace twf {

struct Coordinate { // change to coordinates
    std::size_t x = 0;
    std::size_t y = 0;
};

inline bool operator==(const Coordinate& left, const Coordinate& right) {
    return left.x == right.x && left.y == right.y;
}

}  // namespace twf
#endif  // WIDGET_MODULE_COORDINATE_HPP
