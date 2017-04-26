#include <painter_module/attribute.hpp>
#include <painter_module/color.hpp>
#include <painter_module/geometry.hpp>
#include <painter_module/glyph.hpp>
#include <painter_module/glyph_string.hpp>
#include <painter_module/painter.hpp>
#include <widget_module/border.hpp>
#include <widget_module/coordinate.hpp>
#include <system_module/system.hpp>

#include <cstddef>

namespace twf {

Painter::Painter(Widget* widget) : widget_{widget} {
    this->set_cursor(widget_->cursor());
}

void Painter::put(const Glyph_string& string, bool move_cursor) {
    Coordinate old_position{widget_->cursor_x(), widget_->cursor_y()};
        std::size_t x_border_offset{0};
        std::size_t y_border_offset{0};
        if (widget_->border().enabled() &&
            (widget_->border().west_enabled() || widget_->border().north_west_enabled() ||
             widget_->border().south_west_enabled())) {
            ++x_border_offset;
        }
        if (widget_->border().enabled() &&
            (widget_->border().north_enabled() || widget_->border().north_east_enabled() ||
             widget_->border().north_west_enabled())) {
            ++y_border_offset;
        }

    for (Glyph g : string) {
        add_default_attributes(g);
        std::size_t glob_x =
            widget_->x() + widget_->cursor_x() + x_border_offset;
        std::size_t glob_y =
            widget_->y() + widget_->cursor_y() + y_border_offset;
        if (g.str() == "\n") {
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
        // put into utility global function
        std::size_t x_border_offset{0};
        std::size_t y_border_offset{0};
        if (widget_->border().enabled() &&
            (widget_->border().west_enabled() || widget_->border().north_west_enabled() ||
             widget_->border().south_west_enabled())) {
            ++x_border_offset;
        }
        if (widget_->border().enabled() &&
            (widget_->border().north_enabled() || widget_->border().north_east_enabled() ||
             widget_->border().north_west_enabled())) {
            ++y_border_offset;
        }
        widget_->paint_engine().buffer().cursor_position.x =
            widget_->x() + widget_->cursor_x() + x_border_offset;
        widget_->paint_engine().buffer().cursor_position.y =
            widget_->y() + widget_->cursor_y() + y_border_offset;
    }
}

void Painter::fill(std::size_t x,
                   std::size_t y,
                   std::size_t width,
                   std::size_t height,
                   Color fill_background) {
    if (width == 0 || height == 0) {
        return;
    }
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
            this->move(i, y1,
                       false);  // change to put_at(); but check that bool
            this->put(gs);      // maybe don't change, not as efficient?
        }
    }
    // Vertical
    else if (x1 == x2) {
        for (std::size_t i{y1}; i <= y2; ++i) {
            this->move(x1, i, false);
            this->put(gs);
        }
    }
    // Diagonal not implemented right now

    // Set Widget's cursor position back
    widget_->set_cursor_x(cur_x);
    widget_->set_cursor_y(cur_y);
}

void Painter::border(const Border& b) {
    const std::size_t widg_x = widget_->x();
    const std::size_t widg_y = widget_->y();
    std::size_t width = widget_->geometry().width();
    std::size_t height = widget_->geometry().height();

    // if (widg_y + height > System::max_height()) {
    //     return;
    // }
    // if (widg_x + width > System::max_width()) {
    //     return;
    // }
    // if (widg_x == 0 && (b.north_west_enabled() || b.west_enabled() ||
    //                     b.south_west_enabled()) &&
    //     b.enabled()) {
    //     return;
    // }
    // if (widg_y == 0 && (b.north_enabled() || b.north_west_enabled() ||
    //                     b.north_east_enabled()) &&
    //     b.enabled()) {
    //     return;
    // }
    // if (widg_x + width == 0 && (b.north_enabled() || b.south_enabled())) {
    //     return;
    // }
    // if (widg_y + height == 0 && (b.west_enabled() || b.east_enabled())) {
    //     return;
    // }

    // Checks for making sure size_t's do not go negative
    if (b.enabled() && widg_x + width < 2 && b.north_enabled() &&
        b.south_enabled()) {
        return;
    }
    if (b.enabled() && widg_y + height < 2 && b.east_enabled() &&
        b.west_enabled()) {
        return;
    }

    // North
    if (b.enabled() && b.north_enabled()) {
        unbound_line(widg_x + 1, widg_y, widg_x + width - 2, widg_y, b.north());
    }

    // South
    if (b.enabled() && b.south_enabled()) {
        unbound_line(widg_x + 1, widg_y + height - 1, widg_x + width - 2,
                     widg_y + height - 1, b.south());
    }

    // West
    if (b.enabled() && b.west_enabled()) {
        unbound_line(widg_x, widg_y + 1, widg_x, widg_y + height - 2, b.west());
    }

    // East
    if (b.enabled() && b.east_enabled()) {
        unbound_line(widg_x + width - 1, widg_y + 1, widg_x + width - 1,
                     widg_y + height - 2, b.east());
    }

    // North-West
    if (b.enabled() && b.north_west_enabled()) {
        unbound_put_string(widg_x, widg_y, b.north_west());
    }

    // North-East
    if (b.enabled() && b.north_east_enabled()) {
        unbound_put_string(widg_x + width - 1, widg_y, b.north_east());
    }

    // South-West
    if (b.enabled() && b.south_west_enabled()) {
        unbound_put_string(widg_x, widg_y + height - 1, b.south_west());
    }

    // South-East
    if (b.enabled() && b.south_east_enabled()) {
        unbound_put_string(widg_x + width - 1, widg_y + height - 1,
                           b.south_east());
    }
}

void Painter::border(const Glyph_string& gs) {  // Probably delete this
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

void Painter::unbound_put_string(std::size_t g_x,
                                 std::size_t g_y,
                                 const Glyph_string& gs) {
    for (Glyph g : gs) {
        add_default_attributes(g);
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
        for (std::size_t i{g_x1}; i <= g_x2; ++i) {
            unbound_put_string(i, g_y1, g);
        }
    } else if (g_x1 == g_x2) {
        for (std::size_t i{g_y1}; i <= g_y2; ++i) {
            unbound_put_string(g_x1, i, g);
        }
    }
}

void Painter::add_default_attributes(Glyph& g) {
    if (!g.brush().background_color()) {
        g.brush().add_attributes(
            background(*widget_->brush().background_color()));
    }
    if (!g.brush().foreground_color()) {
        g.brush().add_attributes(
            foreground(*widget_->brush().foreground_color()));
    }
    for (Attribute& attr : widget_->brush().attributes()) {
        g.brush().add_attributes(attr);
    }
}

}  // namespace twf
