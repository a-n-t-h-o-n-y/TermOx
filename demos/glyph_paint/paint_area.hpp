#ifndef DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#define DEMOS_GLYPH_PAINT_PAINT_AREA_HPP
#include <cstddef>
#include <iostream>
#include <map>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/painter.hpp>
#include <termox/painter/trait.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>

namespace paint {

class Paint_area : public ox::Widget {
   public:
    sl::Signal<void(ox::Glyph)> glyph_changed;
    sl::Signal<void()> erase_enabled;
    sl::Signal<void()> erase_disabled;

   public:
    Paint_area() { *this | ox::pipe::strong_focus(); }

   public:
    void set_glyph(ox::Glyph g)
    {
        current_glyph_ = g;
        glyph_changed(std::move(g));
        if (erase_enabled_) {
            this->disable_erase();
            erase_disabled();
        }
    }

    void set_symbol(ox::Glyph s)
    {
        if (erase_enabled_) {
            this->disable_erase();
            erase_disabled();
        }
        current_glyph_.symbol = s.symbol;
        if (s.brush.background != ox::Color::Background)
            current_glyph_ | bg(s.brush.background);
        if (s.brush.foreground != ox::Color::Foreground)
            current_glyph_ | fg(s.brush.foreground);
        glyph_changed(current_glyph_);
    }

    void set_foreground_color(ox::Color c)
    {
        current_glyph_ | fg(c);
        if (!erase_enabled_)
            glyph_changed(current_glyph_);
    }

    void set_background_color(ox::Color c)
    {
        current_glyph_ | bg(c);
        if (!erase_enabled_)
            glyph_changed(current_glyph_);
    }

    void set_trait(ox::Trait t)
    {
        current_glyph_ | t;
        if (!erase_enabled_)
            glyph_changed(current_glyph_);
    }

    void remove_traits(ox::Trait t)
    {
        current_glyph_.brush.traits.remove(t);
        if (!erase_enabled_)
            glyph_changed(current_glyph_);
    }

    void clear()
    {
        glyphs_painted_.clear();
        this->update();
    }

    auto glyph() const -> ox::Glyph { return current_glyph_; }

    void toggle_clone() { clone_enabled_ = !clone_enabled_; }

    void enable_erase()
    {
        erase_enabled_ = true;
        glyph_changed(U' ');
    }

    void disable_erase()
    {
        erase_enabled_ = false;
        glyph_changed(current_glyph_);
    }

    void enable_grid()
    {
        this->set_wallpaper(U'┼' | fg(ox::Color::Dark_gray));
        this->update();
    }

    void disable_grid()
    {
        this->set_wallpaper(U' ');
        this->update();
    }

    void write(std::ostream& os);

    void read(std::istream& is);

   protected:
    auto paint_event(ox::Painter& p) -> bool override
    {
        auto const w = this->width();
        auto const h = this->height();
        for (auto const& [at, glyph] : glyphs_painted_) {
            if (at.x < w && at.y < h)
                p.put(glyph, at);
        }
        return Widget::paint_event(p);
    }

    auto mouse_press_event(ox::Mouse const& m) -> bool override
    {
        switch (m.button) {
            using Button = ox::Mouse::Button;
            case Button::Left: this->place_glyph(m.at); break;
            case Button::Right: this->remove_glyph(m.at); break;
            case Button::Middle:
                if (glyphs_painted_.count(m.at) == 1)
                    this->set_glyph(glyphs_painted_[m.at]);
                break;
            default: break;
        }
        return Widget::mouse_press_event(m);
    }

    auto mouse_move_event(ox::Mouse const& m) -> bool override
    {
        switch (m.button) {
            using Button = ox::Mouse::Button;
            case Button::Left: this->place_glyph(m.at); break;
            case Button::Right: this->remove_glyph(m.at); break;
            case Button::Middle:
                if (glyphs_painted_.count(m.at) == 1)
                    this->set_glyph(glyphs_painted_[m.at]);
                break;
            default: break;
        }
        return Widget::mouse_move_event(m);
    }

    auto key_press_event(ox::Key k) -> bool override;

   private:
    std::map<ox::Point, ox::Glyph> glyphs_painted_;
    ox::Glyph current_glyph_ = U'x';
    ox::Glyph before_erase_  = U'x';
    bool clone_enabled_      = false;
    bool erase_enabled_      = false;

   public:
    void place_glyph(ox::Point p);

    void remove_glyph(ox::Point coords)
    {
        glyphs_painted_.erase(coords);
        this->update();
    }
};

}  // namespace paint

namespace paint::slot {

auto set_symbol(Paint_area& pa) -> sl::Slot<void(ox::Glyph)>;

auto set_foreground_color(Paint_area& pa) -> sl::Slot<void(ox::Color)>;

auto set_background_color(Paint_area& pa) -> sl::Slot<void(ox::Color)>;

auto set_trait(Paint_area& pa, ox::Trait t) -> sl::Slot<void()>;

auto remove_traits(Paint_area& pa, ox::Trait t) -> sl::Slot<void()>;

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
