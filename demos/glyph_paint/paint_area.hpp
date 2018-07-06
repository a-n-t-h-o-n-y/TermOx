#ifndef DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#define DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#include <cppurses/cppurses.hpp>
#include <signals/signals.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>

using namespace cppurses;

namespace demos {
namespace glyph_paint {

class Paint_area : public Widget {
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
    void enable_erase();
    void disable_erase();
    void enable_grid();
    void disable_grid();

    void write(std::ostream& os);
    void read(std::istream& is);

    // Signals
    sig::Signal<void(Glyph)> glyph_changed;
    sig::Signal<void()> erase_enabled;
    sig::Signal<void()> erase_disabled;

   protected:
    bool paint_event(Painter& p) override;

    bool mouse_press_event(Mouse_button button,
                           Point global,
                           Point local,
                           std::uint8_t device_id) override;

    bool key_press_event(Key key, char symbol) override;

   private:
    std::map<Point, Glyph> glyphs_painted_;
    Glyph current_glyph_{L'x'};
    Glyph before_erase_{L'x'};
    bool clone_enabled_{false};
    bool erase_enabled_{false};

    void place_glyph(std::size_t x, std::size_t y);
    void remove_glyph(Point coords);
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

sig::Slot<void()> enable_erase(Paint_area& pa);
sig::Slot<void()> disable_erase(Paint_area& pa);

sig::Slot<void()> enable_grid(Paint_area& pa);
sig::Slot<void()> disable_grid(Paint_area& pa);

sig::Slot<void(std::ostream&)> write(Paint_area& pa);
sig::Slot<void(std::istream&)> read(Paint_area& pa);

}  // namespace slot
}  // namespace glyph_paint
}  // namespace demos
#endif  // DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
