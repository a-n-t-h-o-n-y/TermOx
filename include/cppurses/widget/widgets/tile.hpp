#ifndef CPPURSES_WIDGET_WIDGETS_TILE_HPP
#define CPPURSES_WIDGET_WIDGETS_TILE_HPP
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

class Tile : public Widget {
   public:
    Tile(Glyph g = L' ') : display_{g}
    {
        using namespace pipe;
        *this | fixed_width(1) | fixed_height(1);
    }

    void set(Glyph g)
    {
        display_ = g;
        this->update();
    }

    auto get() -> Glyph { return display_; }

   protected:
    auto paint_event() -> bool override
    {
        auto p = Painter{*this};
        p.put(display_, 0, 0);
        return Widget::paint_event();
    }

   private:
    Glyph display_;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TILE_HPP
