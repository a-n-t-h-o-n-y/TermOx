#ifndef DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#define DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <unordered_map>

#include <signals_light/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/mouse.hpp>
#include <cppurses/widget/widget.hpp>

namespace demos::glyph_paint {

class Paint_area : public cppurses::Widget {
   public:
    Paint_area();

    void set_glyph(cppurses::Glyph glyph);
    void set_symbol(const cppurses::Glyph& symbol);
    void set_foreground_color(cppurses::Color c);
    void set_background_color(cppurses::Color c);
    void set_trait(cppurses::Trait t);
    void remove_traits(cppurses::Trait t);

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
    sl::Signal<void(cppurses::Glyph)> glyph_changed;
    sl::Signal<void()> erase_enabled;
    sl::Signal<void()> erase_disabled;

   protected:
    bool paint_event() override;

    bool mouse_press_event(const cppurses::Mouse& m) override;

    bool key_press_event(cppurses::Key k) override;

   private:
    std::unordered_map<cppurses::Point, cppurses::Glyph> glyphs_painted_;
    cppurses::Glyph current_glyph_{L'x'};
    cppurses::Glyph before_erase_{L'x'};
    bool clone_enabled_{false};
    bool erase_enabled_{false};

    void place_glyph(std::size_t x, std::size_t y);
    void remove_glyph(cppurses::Point coords);
};

}  // namespace demos::glyph_paint

namespace demos::glyph_paint::slot {

auto set_glyph(Paint_area& pa) -> sl::Slot<void(cppurses::Glyph)>;

auto set_glyph(Paint_area& pa, const cppurses::Glyph& glyph)
    -> sl::Slot<void()>;

auto set_symbol(Paint_area& pa) -> sl::Slot<void(cppurses::Glyph)>;

auto set_symbol(Paint_area& pa, const cppurses::Glyph& symbol)
    -> sl::Slot<void()>;

auto set_foreground_color(Paint_area& pa) -> sl::Slot<void(cppurses::Color)>;

auto set_foreground_color(Paint_area& pa, cppurses::Color c)
    -> sl::Slot<void()>;

auto set_background_color(Paint_area& pa) -> sl::Slot<void(cppurses::Color)>;

auto set_background_color(Paint_area& pa, cppurses::Color c)
    -> sl::Slot<void()>;

auto set_trait(Paint_area& pa) -> sl::Slot<void(cppurses::Trait)>;

auto set_trait(Paint_area& pa, cppurses::Trait t) -> sl::Slot<void()>;

auto remove_traits(Paint_area& pa) -> sl::Slot<void(cppurses::Trait)>;

auto remove_traits(Paint_area& pa, cppurses::Trait t) -> sl::Slot<void()>;

auto toggle_clone(Paint_area& pa) -> sl::Slot<void()>;

auto clear(Paint_area& pa) -> sl::Slot<void()>;

auto enable_erase(Paint_area& pa) -> sl::Slot<void()>;

auto disable_erase(Paint_area& pa) -> sl::Slot<void()>;

auto enable_grid(Paint_area& pa) -> sl::Slot<void()>;

auto disable_grid(Paint_area& pa) -> sl::Slot<void()>;

auto write(Paint_area& pa) -> sl::Slot<void(std::ostream&)>;

auto read(Paint_area& pa) -> sl::Slot<void(std::istream&)>;

}  // namespace demos::glyph_paint::slot
#endif  // DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
