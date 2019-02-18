#include <cppurses/widget/widgets/fixed_width.hpp>

#include <cstddef>

namespace cppurses {

Fixed_width::Fixed_width(std::size_t width) {
    this->set_name("Fixed_width");
    this->width_policy.fixed(width);
}
}  // namespace cppurses
