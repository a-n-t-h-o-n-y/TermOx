#ifndef DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#define DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <unordered_map>

#include <signals_light/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/mouse.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>

namespace paint {

class Paint_area : public cppurses::Widget {
   public:
    sl::Signal<void(cppurses::Glyph)> glyph_changed;
    sl::Signal<void()> erase_enabled;
    sl::Signal<void()> erase_disabled;

   public:
    Paint_area()
    {
        using namespace cppurses::pipe;
        *this | strong_focus() | east_border();
    }

   public:
    void set_glyph(cppurses::Glyph g)
    {
        current_glyph_ = g;
        glyph_changed(std::move(g));
        if (erase_enabled_) {
            this->disable_erase();
            erase_disabled();
        }
    }

    void set_symbol(cppurses::Glyph const& symbol)
    {
        if (erase_enabled_) {
            this->disable_erase();
            erase_disabled();
        }
        current_glyph_.symbol = symbol.symbol;
        if (auto const symbol_bg = symbol.brush.background_color(); symbol_bg)
            current_glyph_ | bg(*symbol_bg);
        if (auto const symbol_fg = symbol.brush.foreground_color(); symbol_fg)
            current_glyph_ | fg(*symbol_fg);
        glyph_changed(current_glyph_);
    }

    void set_foreground_color(cppurses::Color c)
    {
        current_glyph_ | fg(c);
        if (!erase_enabled_)
            glyph_changed(current_glyph_);
    }

    void set_background_color(cppurses::Color c)
    {
        current_glyph_ | bg(c);
        if (!erase_enabled_)
            glyph_changed(current_glyph_);
    }

    void set_trait(cppurses::Trait t)
    {
        current_glyph_ | t;
        if (!erase_enabled_)
            glyph_changed(current_glyph_);
    }

    void remove_traits(cppurses::Trait t)
    {
        current_glyph_.brush.remove_traits(t);
        if (!erase_enabled_)
            glyph_changed(current_glyph_);
    }

    void clear()
    {
        glyphs_painted_.clear();
        this->update();
    }

    auto glyph() const -> cppurses::Glyph { return current_glyph_; }

    void toggle_clone() { clone_enabled_ = !clone_enabled_; }

    void enable_erase()
    {
        erase_enabled_ = true;
        glyph_changed(L' ');
    }

    void disable_erase()
    {
        erase_enabled_ = false;
        glyph_changed(current_glyph_);
    }

    void enable_grid()
    {
        this->set_wallpaper(L'┼' | fg(cppurses::Color::Dark_gray));
        this->update();
    }

    void disable_grid()
    {
        this->set_wallpaper(std::nullopt);
        this->update();
    }

    void write(std::ostream& os);

    void read(std::istream& is);

   protected:
    auto paint_event() -> bool override
    {
        auto p       = cppurses::Painter{*this};
        auto const w = this->width();
        auto const h = this->height();
        for (auto const& [at, glyph] : glyphs_painted_) {
            if (at.x < w && at.y < h)
                p.put(glyph, at);
        }
        return Widget::paint_event();
    }

    auto mouse_press_event(cppurses::Mouse const& m) -> bool override
    {
        switch (m.button) {
            using Button = cppurses::Mouse::Button;
            case Button::Right: this->remove_glyph(m.local); break;
            case Button::Middle:
                if (glyphs_painted_.count(m.local) == 1)
                    this->set_glyph(glyphs_painted_[m.local]);
                break;
            default: this->place_glyph(m.local.x, m.local.y); break;
        }
        return Widget::mouse_press_event(m);
    }

    auto key_press_event(cppurses::Key k) -> bool override;

   private:
    std::unordered_map<cppurses::Point, cppurses::Glyph> glyphs_painted_;
    cppurses::Glyph current_glyph_ = L'x';
    cppurses::Glyph before_erase_  = L'x';
    bool clone_enabled_            = false;
    bool erase_enabled_            = false;

   public:
    void place_glyph(std::size_t x, std::size_t y);

    void remove_glyph(cppurses::Point coords)
    {
        glyphs_painted_.erase(coords);
        this->update();
    }
};

}  // namespace paint

namespace paint::slot {

auto set_symbol(Paint_area& pa) -> sl::Slot<void(cppurses::Glyph)>;

auto set_foreground_color(Paint_area& pa) -> sl::Slot<void(cppurses::Color)>;

auto set_background_color(Paint_area& pa) -> sl::Slot<void(cppurses::Color)>;

auto set_trait(Paint_area& pa, cppurses::Trait t) -> sl::Slot<void()>;

auto remove_traits(Paint_area& pa, cppurses::Trait t) -> sl::Slot<void()>;

auto toggle_clone(Paint_area& pa) -> sl::Slot<void()>;

auto clear(Paint_area& pa) -> sl::Slot<void()>;

auto enable_erase(Paint_area& pa) -> sl::Slot<void()>;

auto disable_erase(Paint_area& pa) -> sl::Slot<void()>;

auto enable_grid(Paint_area& pa) -> sl::Slot<void()>;

auto disable_grid(Paint_area& pa) -> sl::Slot<void()>;

auto write(Paint_area& pa) -> sl::Slot<void(std::ostream&)>;

auto read(Paint_area& pa) -> sl::Slot<void(std::istream&)>;

}  // namespace paint::slot
#endif  // DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
