#include <cppurses/widget/point.hpp>

namespace std {

typename hash<cppurses::Point>::result_type hash<cppurses::Point>::operator()(
    const argument_type& point) const noexcept {
    const result_type h1(std::hash<decltype(point.x)>{}(point.x));
    const result_type h2(std::hash<decltype(point.y)>{}(point.y));
    return h1 ^ (h2 << 1);
}

}  // namespace std
