#ifndef CPPURSES_WIDGET_WIDGETS_FIXED_HEIGHT_HPP
#define CPPURSES_WIDGET_WIDGETS_FIXED_HEIGHT_HPP
#include <cstddef>

#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Empty space Widget with fixed height.
struct Fixed_height : Widget {
    explicit Fixed_height(std::size_t height = 1);
};
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_FIXED_HEIGHT_HPP
