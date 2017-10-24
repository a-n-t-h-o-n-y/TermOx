#ifndef DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#define DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#include <cppurses/cppurses.hpp>

#include <cstddef>
#include <cstdint>

using namespace cppurses;

class Paint_area : public Matrix_display {
   public:
    Paint_area() {
        enable_border(*this);
        this->width_policy.stretch(5);
        this->show_cursor();
        this->focus_policy = Focus_policy::Strong;
    }

   protected:
    bool mouse_press_event(Mouse_button button,
                           std::size_t global_x,
                           std::size_t global_y,
                           std::size_t local_x,
                           std::size_t local_y,
                           std::uint8_t device_id) override;

    bool resize_event(std::size_t new_width,
                      std::size_t new_height,
                      std::size_t old_width,
                      std::size_t old_height) override;

    bool key_press_event(Key key, char symbol) override;

   private:
    Glyph current_symbol_{"⊚"};
};

#endif  // DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
