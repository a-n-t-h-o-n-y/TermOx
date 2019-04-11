#include <cppurses/widget/layout.hpp>

namespace cppurses {
namespace layout {

void Layout::update_geometry() {
    this->screen_state().optimize.child_event = true;
}

}  // namespace layout
}  // namespace cppurses
