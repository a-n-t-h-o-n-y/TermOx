#ifndef CPPURSES_WIDGET_AREA_HPP
#define CPPURSES_WIDGET_AREA_HPP
#include <cstddef>

namespace cppurses {

/// Convinience data structure holding Widget size dimensions.
struct Area {
    std::size_t width;
    std::size_t height;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_AREA_HPP
