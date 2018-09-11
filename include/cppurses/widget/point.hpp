#ifndef CPPURSES_WIDGET_POINT_HPP
#define CPPURSES_WIDGET_POINT_HPP
#include <cstddef>
#include <functional>  // std::hash

namespace cppurses {

/// Represents a 2D point in space.
/** Usually taken to be relative to the top-left corner of a Widget, or of the
 *  Terminal screen. */
struct Point {
    std::size_t x = 0;
    std::size_t y = 0;
};

bool operator==(const Point& lhs, const Point& rhs);

bool operator!=(const Point& lhs, const Point& rhs);

bool operator<(const Point& lhs, const Point& rhs);

}  // namespace cppurses

/// Custom specialization of std::hash for cppurses::point.
namespace std {
template <>
struct hash<cppurses::Point> {
    using argument_type = cppurses::Point;
    using result_type = std::size_t;
    result_type operator()(const argument_type& point) const noexcept;
};
}  // namespace std
#endif  // CPPURSES_WIDGET_POINT_HPP
