#ifndef DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#define DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#include <cppurses/cppurses.hpp>

#include <cstddef>
#include <cstdint>

#include <signals/slot.hpp>

using namespace cppurses;

class Paint_area : public Matrix_display {
   public:
    Paint_area();

    void set_symbol(Glyph symbol);

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

namespace slot {

sig::Slot<void(Glyph)> set_symbol(Paint_area& pa);

sig::Slot<void()> set_symbol(Paint_area& pa, const Glyph& symbol);

}  // namespace slot

#endif  // DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
