#ifndef CPPURSES_DEMOS_PALETTE_PALETTE_DEMOS_HPP
#define CPPURSES_DEMOS_PALETTE_PALETTE_DEMOS_HPP
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/widgets/vertical_slider.hpp>

namespace palette {
using namespace cppurses;

class Palette_demo : public Horizontal_layout {
   public:
    Palette_demo();

   private:
    Vertical_slider& slider1{this->make_child<Vertical_slider>(-1000, 1000)};
    Vertical_slider& slider2{this->make_child<Vertical_slider>(0, 255)};
    Vertical_slider& slider3{this->make_child<Vertical_slider>(50, 55)};
};

}  // namespace palette
#endif  // CPPURSES_DEMOS_PALETTE_PALETTE_DEMOS_HPP
