#include "side_panel.hpp"

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/widget/size_policy.hpp>

using namespace cppurses;
namespace gol {

Side_panel::Side_panel() {
    this->width_policy.type(Size_policy::Fixed);
    this->width_policy.hint(16);

    this->border.enable();
    this->border.segments.disable_all();
    this->border.segments.east.enable();
    this->border.segments.east = Glyph{L'│', foreground(Color::Blue)};
}
}  // namespace gol
