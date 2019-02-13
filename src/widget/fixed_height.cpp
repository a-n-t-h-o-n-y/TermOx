#include <cppurses/widget/widgets/fixed_height.hpp>

#include <cstddef>

#include <cppurses/widget/size_policy.hpp>

namespace cppurses {

Fixed_height::Fixed_height(std::size_t height) {
    this->set_name("Fixed_height");
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(height);
}
}  // namespace cppurses
