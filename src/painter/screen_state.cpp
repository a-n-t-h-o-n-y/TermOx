#include <cppurses/painter/detail/screen_state.hpp>

namespace cppurses {
namespace detail {

void Screen_state::Optimize::reset() {
    this->just_enabled = false;
    this->moved = false;
    this->resized = false;
    this->child_event = false;
    this->move_mask.clear();
    this->resize_mask.clear();
}

}  // namespace detail
}  // namespace cppurses
