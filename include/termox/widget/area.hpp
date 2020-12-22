#ifndef TERMOX_WIDGET_AREA_HPP
#define TERMOX_WIDGET_AREA_HPP
#include <cstddef>

namespace ox {

/// Convinience data structure holding Widget size dimensions.
struct Area {
    std::size_t width;
    std::size_t height;
};

/// Compares the dimension values, not the square areas.
inline auto operator==(Area const& x, Area const& y) -> bool
{
    return x.width == y.width && x.height == y.height;
}

/// Compares the dimension values, not the square areas.
inline auto operator!=(Area const& x, Area const& y) -> bool
{
    return !(x == y);
}

/// Compares the square area values, not the individual dimensions.
inline auto operator<(Area const& x, Area const& y) -> bool
{
    return (x.width * x.height) < (y.width * y.height);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_AREA_HPP
