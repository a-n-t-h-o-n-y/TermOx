#include "side_panel.hpp"

#include <cppurses/painter/color.hpp>

using namespace cppurses;
namespace gol {

Side_panel::Side_panel() {
    this->width_policy.fixed(16);
    this->border.enable();
    this->border.segments.disable_all();
    this->border.segments.east.enable();
    this->border.segments.east.brush.set_foreground(Color::Blue);
}
}  // namespace gol
