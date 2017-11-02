#include <cppurses/widget/point.hpp>

namespace cppurses {

bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator<(const Point& lhs, const Point& rhs) {
    if (lhs.y < rhs.y) {
        return true;
    } else if (lhs.y == rhs.y && lhs.x < rhs.x) {
        return true;
    }
    return false;
}

}  // namespace cppurses
