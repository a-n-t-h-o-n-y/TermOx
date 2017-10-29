#ifndef DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#define DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#include <cppurses/cppurses.hpp>

#include <cstddef>
#include <cstdint>

#include <signals/signals.hpp>

using namespace cppurses;

namespace demos {
namespace glyph_paint {

class Paint_area : public Matrix_display {
   public:
    Paint_area();

    void set_glyph(Glyph glyph);
    void set_symbol(const Glyph& symbol);
    void set_foreground_color(Color c);
    void set_background_color(Color c);
    void set_attribute(Attribute attr);
    void remove_attribute(Attribute attr);

    void clear();

    Glyph glyph() const;
    void toggle_clone();

    // Signals
    sig::Signal<void(Glyph)> glyph_changed;

   protected:
    bool resize_event(std::size_t new_width,
                      std::size_t new_height,
                      std::size_t old_width,
                      std::size_t old_height) override;

    bool mouse_press_event(Mouse_button button,
                           std::size_t global_x,
                           std::size_t global_y,
                           std::size_t local_x,
                           std::size_t local_y,
                           std::uint8_t device_id) override;

    bool key_press_event(Key key, char symbol) override;

   private:
    Glyph current_glyph_{"x"};
    bool clone_enabled_{false};

    void place_glyph(std::size_t x, std::size_t y);
};

namespace slot {

sig::Slot<void(Glyph)> set_glyph(Paint_area& pa);
sig::Slot<void()> set_glyph(Paint_area& pa, const Glyph& glyph);

sig::Slot<void(Glyph)> set_symbol(Paint_area& pa);
sig::Slot<void()> set_symbol(Paint_area& pa, const Glyph& symbol);

sig::Slot<void(Color)> set_foreground_color(Paint_area& pa);
sig::Slot<void()> set_foreground_color(Paint_area& pa, Color c);

sig::Slot<void(Color)> set_background_color(Paint_area& pa);
sig::Slot<void()> set_background_color(Paint_area& pa, Color c);

sig::Slot<void(Attribute)> set_attribute(Paint_area& pa);
sig::Slot<void()> set_attribute(Paint_area& pa, Attribute attr);

sig::Slot<void(Attribute)> remove_attribute(Paint_area& pa);
sig::Slot<void()> remove_attribute(Paint_area& pa, Attribute attr);

sig::Slot<void()> toggle_clone(Paint_area& pa);

sig::Slot<void()> clear(Paint_area& pa);

}  // namespace slot
}  // namespace glyph_paint
}  // namespace demos
#endif  // DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
