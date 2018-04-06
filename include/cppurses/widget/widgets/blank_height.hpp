#ifndef WIDGET_WIDGETS_BLANK_HEIGHT_HPP
#define WIDGET_WIDGETS_BLANK_HEIGHT_HPP
#include <cstddef>

#include <cppurses/widget/widget.hpp>

namespace cppurses {

class Blank_height : public Widget {
   public:
    explicit Blank_height(std::size_t height = 1);
};

}  // namespace cppurses
#endif  // WIDGET_WIDGETS_BLANK_HEIGHT_HPP
