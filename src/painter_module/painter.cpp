#include <painter_module/attribute.hpp>
#include <painter_module/color.hpp>
#include <painter_module/geometry.hpp>
#include <painter_module/glyph.hpp>
#include <painter_module/glyph_string.hpp>
#include <painter_module/painter.hpp>
#include <widget_module/border.hpp>

#include <cstddef>

namespace twf {

Painter::Painter(Widget* widget) : widget_{widget} {
    this->set_cursor(widget_->cursor());
    this->move(0, 0);
}

void Painter::put(const Glyph_string& gs) {
    for (Glyph g : gs) {
        if (!g.brush().background_color()) {
            g.brush().add_attributes(
                background(*widget_->brush().background_color()));
        }
        if (!g.brush().foreground_color()) {
            g.brush().add_attributes(
                foreground(*widget_->brush().background_color()));
        }
        for (Attribute& attr : widget_->brush().attributes()) {
            g.brush().add_attributes(attr);
        }

        std::size_t glob_x = widget_->x() + widget_->cursor_x();
        std::size_t glob_y = widget_->y() + widget_->cursor_y();
        widget_->paint_engine().put(glob_x, glob_y, g);
        this->move(widget_->cursor_x() + 1, widget_->cursor_y());
    }
}

void Painter::move(std::size_t x, std::size_t y) {
    widget_->set_cursor_x(x);
    widget_->set_cursor_y(y);

    // If out of widget_'s bounds
    Geometry& geo = widget_->geometry();
    if (x < geo.active_x_min()) {
        this->move(geo.active_x_min(), y);
        return;
    }
    if (x > geo.active_x_max()) {
        this->move(geo.active_x_min(), y + 1);
        return;
    }
    if (y < geo.active_y_min()) {
        this->move(x, geo.active_y_min());
        return;
    }
    if (y > geo.active_y_max()) {
        this->move(x, geo.active_y_max());
        return;
    }
}

void Painter::fill(std::size_t x,
                   std::size_t y,
                   std::size_t width,
                   std::size_t height,
                   Color fill_background) {
    // Save current cursor position
    std::size_t cur_x = widget_->cursor_x();
    std::size_t cur_y = widget_->cursor_y();

    Glyph_string tile{" ", background(fill_background)};
    for (std::size_t i{y}; i < height; ++i) {
        this->line(x, i, width - 1, i, tile);
    }

    // Set Widget's cursor position back
    widget_->set_cursor_x(cur_x);
    widget_->set_cursor_y(cur_y);
}

void Painter::line(std::size_t x1,
                   std::size_t y1,
                   std::size_t x2,
                   std::size_t y2,
                   const Glyph_string& gs) {
    // Save current cursor position
    std::size_t cur_x = widget_->cursor_x();
    std::size_t cur_y = widget_->cursor_y();

    // points must be left to right/ top to bottom right now, implement
    // alternatives.
    // Horizontal
    if (y1 == y2) {
        for (std::size_t i{x1}; i <= x2; ++i) {
            this->move(i, y1);
            this->put(gs);
        }
    }
    // Vertical
    else if (x1 == x2) {
        for (std::size_t i{y1}; i <= y2; ++i) {
            this->move(x1, i);
            this->put(gs);
        }
    }
    // Diagonal not implemented right now

    // Set Widget's cursor position back
    widget_->set_cursor_x(cur_x);
    widget_->set_cursor_y(cur_y);
}

void Painter::border(const Border& b) {
    // Save current cursor position
    std::size_t cur_x = widget_->cursor_x();
    std::size_t cur_y = widget_->cursor_y();

    // North
    this->line(1, 0, widget_->geometry().width() - 2, 0, b.north());

    // South
    this->line(1, widget_->geometry().height() - 1,
               widget_->geometry().width() - 2,
               widget_->geometry().height() - 1, b.south());

    // East
    this->line(widget_->geometry().width() - 1, 1,
               widget_->geometry().width() - 1,
               widget_->geometry().height() - 2, b.east());

    // West
    this->line(0, 1, 0, widget_->geometry().height() - 2, b.west());

    // North - West
    this->move(0, 0);
    this->put(b.north_west());

    // North - East
    this->move(widget_->geometry().width() - 1, 0);
    this->put(b.north_east());

    // South - West
    this->move(0, widget_->geometry().height() - 1);
    this->put(b.south_west());

    // South - East
    this->move(widget_->geometry().width() - 1,
               widget_->geometry().height() - 1);
    this->put(b.south_east());

    // Set Widget's cursor position back
    widget_->set_cursor_x(cur_x);
    widget_->set_cursor_y(cur_y);
}

void Painter::border(const Glyph_string& gs) {  // get rid of this...? {
    // Save current cursor position
    std::size_t cur_x = widget_->cursor_x();
    std::size_t cur_y = widget_->cursor_y();

    // Top border
    this->line(0, 0, widget_->geometry().width() - 1, 0, gs);

    // Bottom border
    this->line(0, widget_->geometry().height() - 1,
               widget_->geometry().width() - 1,
               widget_->geometry().height() - 1, gs);

    // Middle sides
    this->line(0, 0, 0, widget_->geometry().height() - 1, gs);
    this->line(widget_->geometry().width() - 1, 0,
               widget_->geometry().width() - 1,
               widget_->geometry().height() - 1, gs);

    // Set Widget's cursor position back
    widget_->set_cursor_x(cur_x);
    widget_->set_cursor_y(cur_y);
}

void Painter::set_cursor(bool state) {
    if (state) {
        widget_->paint_engine().show_cursor();
    } else {
        widget_->paint_engine().hide_cursor();
    }
}

}  // namespace twf
