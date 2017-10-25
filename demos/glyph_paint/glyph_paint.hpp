#ifndef DEMOS_GLYPH_PAINT_GLYPH_PAINT_HPP
#define DEMOS_GLYPH_PAINT_GLYPH_PAINT_HPP
#include "paint_area.hpp"

#include <cppurses/cppurses.hpp>

using namespace cppurses;

class Glyph_paint : public Horizontal_layout {
   public:
    Glyph_paint();

   private:
    Paint_area& paint_area{this->make_child<Paint_area>()};
    Vertical_layout& side_pane{this->make_child<Vertical_layout>()};
    Glyph_select& glyph_select{side_pane.make_child<Glyph_select>()};
};

namespace slot {

// sig::Slot<void()> 

}  // namespace slot

#endif  // DEMOS_GLYPH_PAINT_GLYPH_PAINT_HPP
