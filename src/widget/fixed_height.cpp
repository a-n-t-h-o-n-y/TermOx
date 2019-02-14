#include <cppurses/widget/widgets/fixed_height.hpp>

#include <cstddef>

namespace cppurses {

Fixed_height::Fixed_height(std::size_t height) {
    this->set_name("Fixed_height");
    this->height_policy.fixed(height);
}
}  // namespace cppurses
