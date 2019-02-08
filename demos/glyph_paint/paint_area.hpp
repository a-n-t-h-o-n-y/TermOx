#ifndef DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#define DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <unordered_map>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/widget.hpp>

#include <signals/signals.hpp>

namespace demos {
namespace glyph_paint {

class Paint_area : public cppurses::Widget {
   public:
    Paint_area();

    void set_glyph(cppurses::Glyph glyph);
    void set_symbol(const cppurses::Glyph& symbol);
    void set_foreground_color(cppurses::Color c);
    void set_background_color(cppurses::Color c);
    void set_attribute(cppurses::Attribute attr);
    void remove_attribute(cppurses::Attribute attr);

    void clear();

    cppurses::Glyph glyph() const;
    void toggle_clone();
    void enable_erase();
    void disable_erase();
    void enable_grid();
    void disable_grid();

    void write(std::ostream& os);
    void read(std::istream& is);

    // Signals
    sig::Signal<void(cppurses::Glyph)> glyph_changed;
    sig::Signal<void()> erase_enabled;
    sig::Signal<void()> erase_disabled;

   protected:
    bool paint_event() override;

    bool mouse_press_event(const cppurses::Mouse_data& mouse) override;

    bool key_press_event(const cppurses::Keyboard_data& keyboard) override;

   private:
    std::unordered_map<cppurses::Point, cppurses::Glyph> glyphs_painted_;
    cppurses::Glyph current_glyph_{L'x'};
    cppurses::Glyph before_erase_{L'x'};
    bool clone_enabled_{false};
    bool erase_enabled_{false};

    void place_glyph(std::size_t x, std::size_t y);
    void remove_glyph(cppurses::Point coords);
};

namespace slot {

sig::Slot<void(cppurses::Glyph)> set_glyph(Paint_area& pa);
sig::Slot<void()> set_glyph(Paint_area& pa, const cppurses::Glyph& glyph);

sig::Slot<void(cppurses::Glyph)> set_symbol(Paint_area& pa);
sig::Slot<void()> set_symbol(Paint_area& pa, const cppurses::Glyph& symbol);

sig::Slot<void(cppurses::Color)> set_foreground_color(Paint_area& pa);
sig::Slot<void()> set_foreground_color(Paint_area& pa, cppurses::Color c);

sig::Slot<void(cppurses::Color)> set_background_color(Paint_area& pa);
sig::Slot<void()> set_background_color(Paint_area& pa, cppurses::Color c);

sig::Slot<void(cppurses::Attribute)> set_attribute(Paint_area& pa);
sig::Slot<void()> set_attribute(Paint_area& pa, cppurses::Attribute attr);

sig::Slot<void(cppurses::Attribute)> remove_attribute(Paint_area& pa);
sig::Slot<void()> remove_attribute(Paint_area& pa, cppurses::Attribute attr);

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
