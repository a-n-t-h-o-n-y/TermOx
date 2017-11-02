#ifndef CPPURSES_WIDGET_POINT_HPP
#define CPPURSES_WIDGET_POINT_HPP
#include <cstddef>

namespace cppurses {

struct Point {
    std::size_t x = 0;
    std::size_t y = 0;
};

bool operator==(const Point& lhs, const Point& rhs);

bool operator<(const Point& lhs, const Point& rhs);

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_POINT_HPP
