#ifndef CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
#define CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
#include <cstdint>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace demos {
namespace animation {

class Animated_bit : public cppurses::Horizontal_layout {
   public:
    Animated_bit(int rate);

    bool animation_event() override;
    bool paint_event(cppurses::Painter& p) override;

    bool mouse_press_event(cppurses::Mouse_button button,
                           cppurses::Point global,
                           cppurses::Point local,
                           std::uint8_t device_id) override;

   private:
    cppurses::Glyph glyph_{'X', cppurses::foreground(cppurses::Color::Yellow)};
    cppurses::Point coords_{0, 0};
    int x_direction{1};
    int y_direction{1};
};

class Animated_widget : public cppurses::Horizontal_layout {
   public:
    Animated_widget() {
        cppurses::enable_border(bit_2_);
        cppurses::enable_border(bit_4_);
    }

   private:
    Animated_bit& bit_1_{this->make_child<Animated_bit>(5)};
    Animated_bit& bit_2_{this->make_child<Animated_bit>(10)};
    Animated_bit& bit_3_{this->make_child<Animated_bit>(20)};
    Animated_bit& bit_4_{this->make_child<Animated_bit>(30)};
    Animated_bit& bit_5_{this->make_child<Animated_bit>(60)};
};

}  // namespace animation
}  // namespace demos
#endif  // CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
