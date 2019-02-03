#include "side_panel.hpp"

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/widget/size_policy.hpp>

using namespace cppurses;
namespace gol {

Side_panel::Side_panel() {
    this->width_policy.type(Size_policy::Fixed);
    this->width_policy.hint(16);
    this->border.enabled = true;
    disable_walls(this->border);
    disable_corners(this->border);
    this->border.east_enabled = true;
    this->border.east = Glyph{L'│', foreground(Color::Blue)};
}
}  // namespace gol
