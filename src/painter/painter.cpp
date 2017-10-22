#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/coordinates.hpp>
#include <cppurses/widget/widget.hpp>

#include <optional/optional.hpp>

#include <cstddef>
#include <cstring>

namespace cppurses {

Painter::Painter(Widget* widget) : widget_{widget} {}

void Painter::put(const Glyph_string& text, std::size_t x, std::size_t y) {
    if (!widget_->on_tree() || !widget_->visible()) {
        return;
    }
    Coordinates original_position{widget_->cursor_x(), widget_->cursor_y()};
    move_cursor(*widget_, x, y);
    for (Glyph g : text) {
        add_default_attributes(&g);
        auto glob_x = widget_->x() + widget_->cursor_x();
        auto glob_y = widget_->y() + widget_->cursor_y();
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
    std::size_t width = widget_->width() + west_border_offset(*widget_);
    std::size_t height = widget_->height() + north_border_offset(*widget_);
    if (width < 1 || height < 1) {
        return;
    }

    const std::size_t widg_x = widget_->x() - west_border_offset(*widget_);
    const std::size_t widg_y = widget_->y() - north_border_offset(*widget_);

    // Underflow Checks
    if (widg_x + width < 1 && (b.north_enabled || b.south_enabled)) {
        return;
    }
    if (widg_y + height < 1 && (b.east_enabled || b.west_enabled)) {
        return;
    }

    // North Wall
    Coordinates north_left{widg_x + 1, widg_y};
    Coordinates north_right{widg_x + width - 1, widg_y};
    // South Wall
    Coordinates south_left{widg_x + 1, widg_y + height};
    Coordinates south_right{widg_x + width - 1, widg_y + height};
    // West Wall
    Coordinates west_top{widg_x, widg_y + 1};
    Coordinates west_bottom{widg_x, widg_y + height - 1};
    // East Wall
    Coordinates east_top{widg_x + width, widg_y + 1};
    Coordinates east_bottom{widg_x + width, widg_y + height - 1};

    // Corners
    Coordinates north_east{east_top.x, north_left.y};
    Coordinates north_west{west_top.x, north_right.y};
    Coordinates south_east{east_bottom.x, south_left.y};
    Coordinates south_west{west_bottom.x, south_right.y};

    // Edge Cases:
    // Height == 1
    if (widget_->height() == 1 && widget_->north_border_disqualified()) {
        west_top = Coordinates{widg_x, widg_y};
        west_bottom = west_top;
        east_top = Coordinates{widg_x + width, widg_y};
        east_bottom = east_top;
    } else if (widget_->height() == 1 && widget_->south_border_disqualified() &&
               !widget_->north_border_disqualified()) {  // && b.north_enabled?
        west_top = Coordinates{widg_x, widg_y + 1};
        west_bottom = west_top;
        east_top = Coordinates{widg_x + width, widg_y + 1};
        east_bottom = east_top;
    }

    // Width == 1
    if (widget_->width() == 1 && widget_->west_border_disqualified()) {
        north_left = Coordinates{widg_x, widg_y};
        north_right = north_left;
        south_left = Coordinates{widg_x, widg_y + height /*- 1*/};
        south_right = south_left;
    } else if (widget_->width() == 1 && widget_->east_border_disqualified() &&
               !widget_->west_border_disqualified()) {  // && b.west_enabled?
        north_left = Coordinates{widg_x + 1, widg_y};
        north_right = north_left;
        south_left = Coordinates{widg_x + 1, widg_y + height /*- 1*/};
        south_right = south_left;
    }

    // North
    if (b.north_enabled && !widget_->north_border_disqualified()) {
        this->unbound_line(north_left, north_right, b.north);
    }
    // South
    if (b.south_enabled && !widget_->south_border_disqualified()) {
        this->unbound_line(south_left, south_right, b.south);
    }
    // West
    if (b.west_enabled && !widget_->west_border_disqualified()) {
        this->unbound_line(west_top, west_bottom, b.west);
    }
    // East
    if (b.east_enabled && !widget_->east_border_disqualified()) {
        this->unbound_line(east_top, east_bottom, b.east);
    }
    // North-West
    if (b.north_west_enabled && !widget_->north_border_disqualified() &&
        !widget_->west_border_disqualified()) {
        this->unbound_put_string(north_west, b.north_west);
    }
    // North-East
    if (b.north_east_enabled && !widget_->north_border_disqualified() &&
        !widget_->east_border_disqualified()) {
        this->unbound_put_string(north_east, b.north_east);
    }
    // South-West
    if (b.south_west_enabled && !widget_->south_border_disqualified() &&
        !widget_->west_border_disqualified()) {
        this->unbound_put_string(south_west, b.south_west);
    }
    // South-East
    if (b.south_east_enabled && !widget_->south_border_disqualified() &&
        !widget_->east_border_disqualified()) {
        this->unbound_put_string(south_east, b.south_east);
    }

    // Stop out of bounds drawing for special cases.
    if (widget_->height() == 1 && widget_->north_border_disqualified() &&
        widget_->south_border_disqualified()) {
        return;
    }
    if (widget_->width() == 1 && widget_->west_border_disqualified() &&
        widget_->east_border_disqualified()) {
        return;
    }

    // Corners - Special Cases
    // North-West
    Coordinates nw{widget_->x() - west_border_offset(*widget_),
                   widget_->y() - north_border_offset(*widget_)};
    if (!b.north_west_enabled && !b.north_enabled && b.west_enabled) {
        this->unbound_put_string(nw, b.west);
    } else if (!b.north_west_enabled && !b.west_enabled && b.north_enabled) {
        this->unbound_put_string(nw, b.north);
    }
    // North-East
    Coordinates ne{
        widget_->x() + widget_->width() - 1 + east_border_offset(*widget_),
        widget_->y() - north_border_offset(*widget_)};
    if (!b.north_east_enabled && !b.north_enabled && b.east_enabled) {
        this->unbound_put_string(ne, b.east);
    } else if (!b.north_east_enabled && !b.east_enabled && b.north_enabled) {
        this->unbound_put_string(ne, b.north);
    }
    // South-West
    Coordinates sw{
        widget_->x() - west_border_offset(*widget_),
        widget_->y() + widget_->height() - 1 + south_border_offset(*widget_)};
    if (!b.south_west_enabled && !b.south_enabled && b.west_enabled) {
        this->unbound_put_string(sw, b.west);
    } else if (!b.south_west_enabled && !b.west_enabled && b.south_enabled) {
        this->unbound_put_string(sw, b.south);
    }
    // South-East
    Coordinates se{
        widget_->x() + widget_->width() - 1 + east_border_offset(*widget_),
        widget_->y() + widget_->height() - 1 + south_border_offset(*widget_)};
    if (!b.south_east_enabled && !b.south_enabled && b.east_enabled) {
        this->unbound_put_string(se, b.east);
    } else if (!b.south_east_enabled && !b.east_enabled && b.south_enabled) {
        this->unbound_put_string(se, b.south);
    }
}

void Painter::clear_screen() {
    auto width = widget_->width() + west_border_offset(*widget_) +
                 east_border_offset(*widget_);
    auto height = widget_->height() + north_border_offset(*widget_) +
                  south_border_offset(*widget_);
    auto gx = widget_->x() - west_border_offset(*widget_);
    auto gy = widget_->y() - north_border_offset(*widget_);

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

void Painter::unbound_put_string(const Coordinates& point,
                                 const Glyph_string& gs) {
    this->unbound_put_string(point.x, point.y, gs);
}

void Painter::unbound_put_string(std::size_t glob_x,
                                 std::size_t glob_y,
                                 const Glyph_string& gs) {
    if (!widget_->on_tree() || !widget_->visible()) {
        return;
    }
    for (Glyph g : gs) {
        add_default_attributes(&g);
        System::paint_buffer()->stage(glob_x++, glob_y, g);
    }
}

void Painter::unbound_line(const Coordinates& point_1,
                           const Coordinates& point_2,
                           const Glyph& symbol) {
    this->unbound_line(point_1.x, point_1.y, point_2.x, point_2.y, symbol);
}

void Painter::unbound_line(std::size_t glob_x1,
                           std::size_t glob_y1,
                           std::size_t glob_x2,
                           std::size_t glob_y2,
                           const Glyph& symbol) {
    // Horizontal
    if (glob_y1 == glob_y2) {
        for (; glob_x1 <= glob_x2; ++glob_x1) {
            unbound_put_string(glob_x1, glob_y1, symbol);
        }
    } else if (glob_x1 == glob_x2) {
        for (; glob_y1 <= glob_y2; ++glob_y1) {
            unbound_put_string(glob_x1, glob_y1, symbol);
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
