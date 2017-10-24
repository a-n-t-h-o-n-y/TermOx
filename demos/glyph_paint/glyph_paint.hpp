#ifndef DEMOS_GLYPH_PAINT_GLYPH_PAINT_HP
#define DEMOS_GLYPH_PAINT_GLYPH_PAINT_HP
#include "paint_area.hpp"

#include <cppurses/cppurses.hpp>

using namespace cppurses;

class Glyph_paint : public Horizontal_layout {
   public:
   private:
    Paint_area& paint_area{this->make_child<Paint_area>()};
    Vertical_layout& side_pane{this->make_child<Vertical_layout>()};
    Slider& slider{side_pane.make_child<Slider>()};
};

#endif  // DEMOS_GLYPH_PAINT_GLYPH_PAINT_HP
