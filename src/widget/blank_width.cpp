#include "widget/widgets/blank_width.hpp"

#include <cstddef>

namespace cppurses {

Blank_width::Blank_width(std::size_t width) {
    this->width_policy.type(Size_policy::Fixed);
    this->width_policy.hint(width);
}

}  // namespace cppurses
