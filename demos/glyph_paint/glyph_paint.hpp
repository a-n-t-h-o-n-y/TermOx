#ifndef DEMOS_GLYPH_PAINT_GLYPH_PAINT_HPP
#define DEMOS_GLYPH_PAINT_GLYPH_PAINT_HPP
#include "paint_area.hpp"
#include "side_pane.hpp"

#include <termox/widget/layouts/horizontal.hpp>

namespace paint {

class Glyph_paint : public ox::layout::Horizontal<> {
   public:
    Glyph_paint();

   private:
    Paint_area& paint_area = make_child<Paint_area>();
    Side_pane& side_pane   = make_child<Side_pane>();
};

}  // namespace paint
#endif  // DEMOS_GLYPH_PAINT_GLYPH_PAINT_HPP
