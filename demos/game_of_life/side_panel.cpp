#include "side_panel.hpp"

#include <cppurses/widget/size_policy.hpp>

namespace gol {

Side_panel::Side_panel() {
    this->width_policy.type(cppurses::Size_policy::Fixed);
    this->width_policy.hint(16);
    this->border.enabled = true;
    disable_walls(this->border);
    disable_corners(this->border);
    this->border.east_enabled = true;
}
}  // namespace gol
