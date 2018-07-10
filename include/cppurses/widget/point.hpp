#ifndef CPPURSES_WIDGET_POINT_HPP
#define CPPURSES_WIDGET_POINT_HPP
#include <cstddef>
#include <functional>  // std::hash

namespace cppurses {

struct Point {
    std::size_t x = 0;
    std::size_t y = 0;
};

bool operator==(const Point& lhs, const Point& rhs);

bool operator<(const Point& lhs, const Point& rhs);

}  // namespace cppurses

/// Custom specialization of std::hash
namespace std {
template <>
struct hash<cppurses::Point> {
    using argument_type = cppurses::Point;
    using result_type = std::size_t;
    result_type operator()(const argument_type& point) const noexcept {
        const result_type h1(std::hash<std::size_t>{}(point.x));
        const result_type h2(std::hash<std::size_t>{}(point.y));
        return h1 ^ (h2 << 1);
    }
};
}  // namespace std

#endif  // CPPURSES_WIDGET_POINT_HPP
