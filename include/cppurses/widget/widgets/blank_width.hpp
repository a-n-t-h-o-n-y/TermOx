#ifndef CPPURSES_WIDGET_WIDGETS_BLANK_WIDTH_HPP
#define CPPURSES_WIDGET_WIDGETS_BLANK_WIDTH_HPP
#include <cstddef>

#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Empty space Widget with fixed width.
class Blank_width : public Widget {
   public:
    explicit Blank_width(std::size_t width = 1);
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_BLANK_WIDTH_HPP
