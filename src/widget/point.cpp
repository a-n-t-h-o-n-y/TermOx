#include <cppurses/widget/point.hpp>

namespace cppurses {

bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(const Point& lhs, const Point& rhs) {
    return !(lhs == rhs);
}

bool operator<(const Point& lhs, const Point& rhs) {
    if (lhs.y < rhs.y) {
        return true;
    }
    if (lhs.y == rhs.y && lhs.x < rhs.x) {
        return true;
    }
    return false;
}

}  // namespace cppurses

namespace std {
typename hash<cppurses::Point>::result_type hash<cppurses::Point>::operator()(
    const argument_type& point) const noexcept {
    const result_type h1(std::hash<decltype(point.x)>{}(point.x));
    const result_type h2(std::hash<decltype(point.y)>{}(point.y));
    return h1 ^ (h2 << 1);
}
}  // namespace std
