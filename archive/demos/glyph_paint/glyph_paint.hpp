#ifndef DEMOS_GLYPH_PAINT_GLYPH_PAINT_HPP
#define DEMOS_GLYPH_PAINT_GLYPH_PAINT_HPP
#include "paint_area.hpp"
#include "side_pane.hpp"

#include <termox/widget/pair.hpp>
#include <termox/widget/pipe.hpp>

namespace paint {

class Glyph_paint : public ox::HPair<ox::Bordered<Paint_area>, Side_pane> {
   public:
    Glyph_paint();

   private:
    Paint_area& paint_area =
        this->first | ox::pipe::take_east() | ox::pipe::wrapped();
    Side_pane& side_pane = this->second;
};

}  // namespace paint
#endif  // DEMOS_GLYPH_PAINT_GLYPH_PAINT_HPP
