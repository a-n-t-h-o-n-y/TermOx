#ifndef CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
#define CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace demos {
namespace animation {

class Animated_bit : public cppurses::Widget {
   public:
    Animated_bit(int rate, bool ani = false);

    bool timer_event() override;
    bool paint_event() override;

    bool mouse_press_event(const cppurses::Mouse::State& mouse) override;

   private:
    cppurses::Glyph glyph_{'X', cppurses::foreground(cppurses::Color::Yellow)};
    cppurses::Point coords_{0, 0};
    int x_direction{1};
    int y_direction{1};
};

class Animated_widget : public cppurses::layout::Horizontal<Animated_bit> {
   public:
    Animated_widget()
    {
        bit_2_.border.enable();
        bit_4_.border.enable();
    }

   private:
    Animated_bit& bit_1_{this->make_child(5, true)};
    Animated_bit& bit_2_{this->make_child(10, true)};
    Animated_bit& bit_3_{this->make_child(20, true)};
    Animated_bit& bit_4_{this->make_child(30, true)};
    Animated_bit& bit_5_{this->make_child(60, true)};
};
}  // namespace animation
}  // namespace demos
#endif  // CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
