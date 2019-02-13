#include <cppurses/widget/widgets/fixed_width.hpp>

#include <cstddef>

#include <cppurses/widget/size_policy.hpp>

namespace cppurses {

Fixed_width::Fixed_width(std::size_t width) {
    this->set_name("Fixed_width");
    this->width_policy.type(Size_policy::Fixed);
    this->width_policy.hint(width);
}
}  // namespace cppurses
