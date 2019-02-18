#ifndef CPPURSES_WIDGET_WIDGETS_FIXED_WIDTH_HPP
#define CPPURSES_WIDGET_WIDGETS_FIXED_WIDTH_HPP
#include <cstddef>

#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Empty space Widget with fixed width.
struct Fixed_width : Widget {
    explicit Fixed_width(std::size_t width = 1);
};
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_FIXED_WIDTH_HPP
