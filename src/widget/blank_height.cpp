#include "widget/widgets/blank_height.hpp"

namespace cppurses {

Blank_height::Blank_height(std::size_t height) {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(height);
}

}  // namespace cppurses
