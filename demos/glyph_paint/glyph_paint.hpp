#ifndef DEMOS_GLYPH_PAINT_GLYPH_PAINT_HPP
#define DEMOS_GLYPH_PAINT_GLYPH_PAINT_HPP
#include "paint_area.hpp"
#include "side_pane.hpp"

#include <cppurses/widget/layouts/horizontal_layout.hpp>

namespace demos {
namespace glyph_paint {

class Glyph_paint : public cppurses::Horizontal_layout {
   public:
    Glyph_paint();

   private:
    Paint_area& paint_area{this->make_child<Paint_area>()};
    Side_pane& side_pane{this->make_child<Side_pane>()};
};

}  // namespace glyph_paint
}  // namespace demos
#endif  // DEMOS_GLYPH_PAINT_GLYPH_PAINT_HPP
