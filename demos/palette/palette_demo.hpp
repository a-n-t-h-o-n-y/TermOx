#ifndef CPPURSES_DEMOS_PALETTE_PALETTE_DEMOS_HPP
#define CPPURSES_DEMOS_PALETTE_PALETTE_DEMOS_HPP
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/widgets/vertical_slider.hpp>

namespace palette {
using namespace cppurses;

class Palette_demo : public Horizontal_layout {
   public:
   private:
    Vertical_slider& slider{this->make_child<Vertical_slider>(256)};
};

}  // namespace palette
#endif  // CPPURSES_DEMOS_PALETTE_PALETTE_DEMOS_HPP
