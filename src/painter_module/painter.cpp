#include "painter_module/painter.hpp"
#include "painter_module/color.hpp"
#include "painter_module/glyph.hpp"
#include "painter_module/glyph_string.hpp"
#include "widget_module/border.hpp"
#include "widget_module/coordinate.hpp"
#include "widget_module/widget.hpp"
#include <cstddef>
#include <cstring>

namespace twf {

Painter::Painter(Widget* widget) : widget_{widget} {
    this->set_cursor(widget_->cursor());
}

void Painter::put(const Glyph_string& string, bool move_cursor) {
    Coordinate old_position{widget_->cursor_x(), widget_->cursor_y()};
    for (Glyph g : string) {
        add_default_attributes(&g);
        std::size_t glob_x = widget_->x() + widget_->cursor_x() +
                             west_border_offset(widget_->border());
        std::size_t glob_y = widget_->y() + widget_->cursor_y() +
                             north_border_offset(widget_->border());
        if (std::strcmp(g.c_str(), "\n") == 0) {
            this->move(0, widget_->cursor_y() + 1, move_cursor);
        } else {
            widget_->paint_engine().put(glob_x, glob_y, g);
            this->move(widget_->cursor_x() + 1, widget_->cursor_y(),
                       move_cursor);
        }
    }
    if (!move_cursor) {
        this->move(old_position.x, old_position.y, false);
    }
    widget_->paint_engine().clear_attributes();
}

void Painter::put_at(Coordinate pos,
                     const Glyph_string& string,
                     bool move_cursor) {
    this->put_at(pos.x, pos.y, string, move_cursor);
}

void Painter::put_at(std::size_t x,
                     std::size_t y,
                     const Glyph_string& string,
                     bool move_cursor) {
    this->move(x, y, move_cursor);
    this->put(string, move_cursor);
}

void Painter::move(Coordinate pos, bool update_buffer) {
    this->move(pos.x, pos.y, update_buffer);
}

void Painter::move(std::size_t x, std::size_t y, bool update_buffer) {
    // Adjust coordinates if out of widget_'s bounds
    if (x >= widget_->width()) {
        x = 0;
        ++y;
    }
    if (y >= widget_->height()) {
        y = widget_->height() - 1;
    }
    widget_->set_cursor_x(x);
    widget_->set_cursor_y(y);
    // Move cursor on screen
    if (update_buffer) {
        widget_->paint_engine().buffer().cursor_position.x =
            widget_->x() + widget_->cursor_x() +
            west_border_offset(widget_->border());
        widget_->paint_engine().buffer().cursor_position.y =
            widget_->y() + widget_->cursor_y() +
            north_border_offset(widget_->border());
    }
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
            this->put_at(x1, y1, gs, false);
        }
    } else if (x1 == x2) {  // Vertical
        for (; y1 <= y2; ++y1) {
            this->put_at(x1, y1, gs, false);
        }
    }
}

void Painter::border(const Border& b) {
    if (!b.enabled()) {
        return;
    }
    const std::size_t widg_x = widget_->x();
    const std::size_t widg_y = widget_->y();
    std::size_t width = widget_->geometry().width();
    std::size_t height = widget_->geometry().height();

    // Checks for making sure size_t's do not go negative
    if (widg_x + width < 2 && (b.north_enabled() || b.south_enabled())) {
        return;
    }
    if (widg_y + height < 2 && (b.east_enabled() || b.west_enabled())) {
        return;
    }

    // North
    if (b.north_enabled()) {
        this->unbound_line(widg_x + 1, widg_y, widg_x + width - 2, widg_y,
                           b.north());
    }

    // South
    if (b.south_enabled()) {
        this->unbound_line(widg_x + 1, widg_y + height - 1, widg_x + width - 2,
                           widg_y + height - 1, b.south());
    }

    // West
    if (b.west_enabled()) {
        this->unbound_line(widg_x, widg_y + 1, widg_x, widg_y + height - 2,
                           b.west());
    }

    // East
    if (b.east_enabled()) {
        this->unbound_line(widg_x + width - 1, widg_y + 1, widg_x + width - 1,
                           widg_y + height - 2, b.east());
    }

    // North-West
    if (b.north_west_enabled()) {
        this->unbound_put_string(widg_x, widg_y, b.north_west());
    }

    // North-East
    if (b.north_east_enabled()) {
        this->unbound_put_string(widg_x + width - 1, widg_y, b.north_east());
    }

    // South-West
    if (b.south_west_enabled()) {
        this->unbound_put_string(widg_x, widg_y + height - 1, b.south_west());
    }

    // South-East
    if (b.south_east_enabled()) {
        this->unbound_put_string(widg_x + width - 1, widg_y + height - 1,
                                 b.south_east());
    }
}

void Painter::set_cursor(bool enabled) {
    if (enabled) {
        widget_->paint_engine().show_cursor();
    } else {
        widget_->paint_engine().hide_cursor();
    }
}

void Painter::clear_screen() {
    auto width = widget_->geometry().width();
    auto height = widget_->geometry().height();
    auto gx = widget_->x();
    auto gy = widget_->y();

    if (width == 0 || height == 0) {
        return;
    }
    Glyph bg_tile = widget_->background_tile();
    if (!bg_tile.brush().background_color() &&
        widget_->brush().background_color()) {
        bg_tile.brush().set_background(*widget_->brush().background_color());
    }
    if (!bg_tile.brush().foreground_color() &&
        widget_->brush().foreground_color()) {
        bg_tile.brush().set_foreground(*widget_->brush().foreground_color());
    }
    for (std::size_t i{gy}; i < gy + height; ++i) {
        this->unbound_line(gx, i, gx + width - 1, i, bg_tile);
    }
}

void Painter::unbound_put_string(std::size_t g_x,
                                 std::size_t g_y,
                                 const Glyph_string& gs) {
    for (Glyph g : gs) {
        add_default_attributes(&g);
        widget_->paint_engine().put(g_x++, g_y, g);
    }
    widget_->paint_engine().clear_attributes();
}

void Painter::unbound_line(std::size_t g_x1,
                           std::size_t g_y1,
                           std::size_t g_x2,
                           std::size_t g_y2,
                           const Glyph& g) {
    // Horizontal
    if (g_y1 == g_y2) {
        for (; g_x1 <= g_x2; ++g_x1) {
            unbound_put_string(g_x1, g_y1, g);
        }
    } else if (g_x1 == g_x2) {
        for (; g_y1 <= g_y2; ++g_y1) {
            unbound_put_string(g_x1, g_y1, g);
        }
    }
}

void Painter::add_default_attributes(Glyph* g) {
    if (!g->brush().background_color() && widget_->brush().background_color()) {
        g->brush().add_attributes(
            background(*widget_->brush().background_color()));
    }
    if (!g->brush().foreground_color() && widget_->brush().foreground_color()) {
        g->brush().add_attributes(
            foreground(*widget_->brush().foreground_color()));
    }
    for (const auto& attr : widget_->brush().attributes()) {
        g->brush().add_attributes(attr);
    }
}

}  // namespace twf
