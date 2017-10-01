#include "painter/painter.hpp"
#include <cstddef>
#include <cstring>
#include "painter/brush.hpp"
#include "painter/color.hpp"
#include "painter/glyph.hpp"
#include "painter/glyph_string.hpp"
#include "painter/paint_buffer.hpp"
#include "widget/border.hpp"
#include "widget/coordinates.hpp"
#include "widget/widget.hpp"
#include "system/system.hpp"
#include <optional/optional.hpp>

namespace cppurses {

Painter::Painter(Widget* widget) : widget_{widget} {}

void Painter::put(const Glyph_string& text, std::size_t x, std::size_t y) {
    Coordinates original_position{widget_->cursor_x(), widget_->cursor_y()};
    move_cursor(*widget_, x, y);
    for (Glyph g : text) {
        add_default_attributes(&g);
        auto glob_x = west_border_offset(widget_->border) + widget_->x() +
                      widget_->cursor_x();
        auto glob_y = north_border_offset(widget_->border) + widget_->y() +
                      widget_->cursor_y();
        if (std::strcmp(g.c_str(), "\n") == 0) {
            move_cursor(*widget_, 0, widget_->cursor_y() + 1);
        }  // TODO else if ( == \t) then move to next x coord divisible by
           // tabspace
           // should be here and textbox should just account for it.
        else {
            System::paint_buffer()->stage(glob_x, glob_y, g);
            move_cursor(*widget_, widget_->cursor_x() + 1, widget_->cursor_y());
        }
    }
    if (!move_cursor_on_put) {
        move_cursor(*widget_, original_position.x, original_position.y);
    }
}

void Painter::put(const Glyph_string& text, Coordinates position) {
    this->put(text, position.x, position.y);
}

void Painter::put(const Glyph_string& text) {
    this->put(text, widget_->cursor_x(), widget_->cursor_y());
}

void Painter::fill(std::size_t x,
                   std::size_t y,
                   std::size_t width,
                   std::size_t height,
                   const Glyph& tile) {
    if (width == 0) {
        return;
    }
    for (; y < height; ++y) {
        this->line(x, y, width - 1, y, tile);
    }
}

void Painter::line(std::size_t x1,
                   std::size_t y1,
                   std::size_t x2,
                   std::size_t y2,
                   const Glyph_string& gs) {
    // No diagonal lines atm.
    if (y1 == y2) {  // Horizontal
        for (; x1 <= x2; ++x1) {
            this->put(gs, x1, y1);
        }
    } else if (x1 == x2) {  // Vertical
        for (; y1 <= y2; ++y1) {
            this->put(gs, x1, y1);
        }
    }
}

void Painter::border(const Border& b) {
    if (!b.enabled) {
        return;
    }
    const std::size_t widg_x = widget_->x();
    const std::size_t widg_y = widget_->y();
    std::size_t width = widget_->width() + west_border_offset(widget_->border) +
                        east_border_offset(widget_->border);
    std::size_t height = widget_->height() +
                         north_border_offset(widget_->border) +
                         south_border_offset(widget_->border);

    // Underflow Checks
    if (widg_x + width < 2 && (b.north_enabled || b.south_enabled)) {
        return;
    }
    if (widg_y + height < 2 && (b.east_enabled || b.west_enabled)) {
        return;
    }
    // North
    if (b.north_enabled) {
        this->unbound_line(widg_x + 1, widg_y, widg_x + width - 2, widg_y,
                           b.north);
    }
    // South
    if (b.south_enabled) {
        this->unbound_line(widg_x + 1, widg_y + height - 1, widg_x + width - 2,
                           widg_y + height - 1, b.south);
    }
    // West
    if (b.west_enabled) {
        this->unbound_line(widg_x, widg_y + 1, widg_x, widg_y + height - 2,
                           b.west);
    }
    // East
    if (b.east_enabled) {
        this->unbound_line(widg_x + width - 1, widg_y + 1, widg_x + width - 1,
                           widg_y + height - 2, b.east);
    }
    // North-West
    if (b.north_west_enabled) {
        this->unbound_put_string(b.north_west, widg_x, widg_y);
    }
    // North-East
    if (b.north_east_enabled) {
        this->unbound_put_string(b.north_east, widg_x + width - 1, widg_y);
    }
    // South-West
    if (b.south_west_enabled) {
        this->unbound_put_string(b.south_west, widg_x, widg_y + height - 1);
    }
    // South-East
    if (b.south_east_enabled) {
        this->unbound_put_string(b.south_east, widg_x + width - 1,
                                 widg_y + height - 1);
    }
}

void Painter::clear_screen() {
    auto width = widget_->width() + west_border_offset(widget_->border) +
                 east_border_offset(widget_->border);
    auto height = widget_->height() + north_border_offset(widget_->border) +
                  south_border_offset(widget_->border);
    auto gx = widget_->x();
    auto gy = widget_->y();

    if (width == 0 || height == 0) {
        return;
    }
    Glyph bg_tile = widget_->background_tile;
    if (!bg_tile.brush().background_color() &&
        widget_->brush.background_color()) {
        bg_tile.brush().set_background(*widget_->brush.background_color());
    }
    if (!bg_tile.brush().foreground_color() &&
        widget_->brush.foreground_color()) {
        bg_tile.brush().set_foreground(*widget_->brush.foreground_color());
    }
    for (std::size_t i{gy}; i < gy + height; ++i) {
        this->unbound_line(gx, i, gx + width - 1, i, bg_tile);
    }
}

void Painter::unbound_put_string(const Glyph_string& gs,
                                 std::size_t glob_x,
                                 std::size_t glob_y) {
    for (Glyph g : gs) {
        add_default_attributes(&g);
        System::paint_buffer()->stage(glob_x++, glob_y, g);
    }
}

void Painter::unbound_line(std::size_t glob_x1,
                           std::size_t glob_y1,
                           std::size_t glob_x2,
                           std::size_t glob_y2,
                           const Glyph& symbol) {
    // Horizontal
    if (glob_y1 == glob_y2) {
        for (; glob_x1 <= glob_x2; ++glob_x1) {
            unbound_put_string(symbol, glob_x1, glob_y1);
        }
    } else if (glob_x1 == glob_x2) {
        for (; glob_y1 <= glob_y2; ++glob_y1) {
            unbound_put_string(symbol, glob_x1, glob_y1);
        }
    }
}

void Painter::add_default_attributes(Glyph* g) {
    if (!g->brush().background_color() && widget_->brush.background_color()) {
        g->brush().add_attributes(
            background(*widget_->brush.background_color()));
    }
    if (!g->brush().foreground_color() && widget_->brush.foreground_color()) {
        g->brush().add_attributes(
            foreground(*widget_->brush.foreground_color()));
    }
    for (const auto& attr : widget_->brush.attributes()) {
        g->brush().add_attributes(attr);
    }
}

}  // namespace cppurses
