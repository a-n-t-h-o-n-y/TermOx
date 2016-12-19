#include <painter_module/glyph.hpp>
#include <painter_module/painter.hpp>
#include <widget_module/border.hpp>

namespace mcurses {

void Border::set_walls(const Glyph& horizontals, const Glyph& verticals) {
    north_ = horizontals;
    south_ = horizontals;
    east_ = verticals;
    west_ = verticals;
}

}  // namespace mcurses
