#include <cppurses/painter/painter.hpp>

#include <cstddef>
#include <cstring>

#include <optional/optional.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Painter::Painter(Widget* widg)
    : widget_{widg},
      staged_changes_{
          System::find_event_loop().staged_changes()[widg].screen_description} {
}

Painter::Painter(Widget* widg, detail::Staged_changes& changes)
    : widget_{widg}, staged_changes_{changes[widg].screen_description} {}

void Painter::put(const Glyph& tile, std::size_t x, std::size_t y) {
    if (x >= widget_->outer_width() || y >= widget_->outer_height()) {
        return;
    }
    std::size_t glob_x = widget_->inner_x() + x;
    std::size_t glob_y = widget_->inner_y() + y;
    this->put_global(tile, glob_x, glob_y);
}

void Painter::put(const Glyph& tile, Point position) {
    this->put(tile, position.x, position.y);
}

void Painter::put(const Glyph_string& text, std::size_t x, std::size_t y) {
    if (!widget_->on_tree() || !widget_->visible()) {
        return;
    }
    for (const Glyph& g : text) {
        this->put(g, x++, y);
    }
}

void Painter::put(const Glyph_string& text, Point position) {
    this->put(text, position.x, position.y);
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
    const std::size_t widg_x = widget_->x();
    const std::size_t widg_y = widget_->y();

    // Underflow Checks
    if (widg_x + width < 1 && (b.north_enabled || b.south_enabled)) {
        return;
    }
    if (widg_y + height < 1 && (b.east_enabled || b.west_enabled)) {
        return;
    }

    // North Wall
    Point north_left{widg_x + 1, widg_y};
    Point north_right{widg_x + width - 1, widg_y};
    // South Wall
    Point south_left{widg_x + 1, widg_y + height};
    Point south_right{widg_x + width - 1, widg_y + height};
    // West Wall
    Point west_top{widg_x, widg_y + 1};
    Point west_bottom{widg_x, widg_y + height - 1};
    // East Wall
    Point east_top{widg_x + width, widg_y + 1};
    Point east_bottom{widg_x + width, widg_y + height - 1};

    // Corners
    Point north_east{east_top.x, north_left.y};
    Point north_west{west_top.x, north_right.y};
    Point south_east{east_bottom.x, south_left.y};
    Point south_west{west_bottom.x, south_right.y};

    // Edge Cases:
    // Height == 1
    if (widget_->height() == 1 && widget_->north_border_disqualified()) {
        west_top = Point{widg_x, widg_y};
        west_bottom = west_top;
        east_top = Point{widg_x + width, widg_y};
        east_bottom = east_top;
    } else if (widget_->height() == 1 && widget_->south_border_disqualified() &&
               !widget_->north_border_disqualified()) {  // && b.north_enabled?
        west_top = Point{widg_x, widg_y + 1};
        west_bottom = west_top;
        east_top = Point{widg_x + width, widg_y + 1};
        east_bottom = east_top;
    }

    // Width == 1
    if (widget_->width() == 1 && widget_->west_border_disqualified()) {
        north_left = Point{widg_x, widg_y};
        north_right = north_left;
        south_left = Point{widg_x, widg_y + height /*- 1*/};
        south_right = south_left;
    } else if (widget_->width() == 1 && widget_->east_border_disqualified() &&
               !widget_->west_border_disqualified()) {  // && b.west_enabled?
        north_left = Point{widg_x + 1, widg_y};
        north_right = north_left;
        south_left = Point{widg_x + 1, widg_y + height /*- 1*/};
        south_right = south_left;
    }

    // North
    if (b.north_enabled && !widget_->north_border_disqualified()) {
        this->line_global(b.north, north_left, north_right);
    }
    // South
    if (b.south_enabled && !widget_->south_border_disqualified()) {
        this->line_global(b.south, south_left, south_right);
    }
    // West
    if (b.west_enabled && !widget_->west_border_disqualified()) {
        this->line_global(b.west, west_top, west_bottom);
    }
    // East
    if (b.east_enabled && !widget_->east_border_disqualified()) {
        this->line_global(b.east, east_top, east_bottom);
    }
    // North-West
    if (b.north_west_enabled && !widget_->north_border_disqualified() &&
        !widget_->west_border_disqualified()) {
        this->put_global(b.north_west, north_west);
    }
    // North-East
    if (b.north_east_enabled && !widget_->north_border_disqualified() &&
        !widget_->east_border_disqualified()) {
        this->put_global(b.north_east, north_east);
    }
    // South-West
    if (b.south_west_enabled && !widget_->south_border_disqualified() &&
        !widget_->west_border_disqualified()) {
        this->put_global(b.south_west, south_west);
    }
    // South-East
    if (b.south_east_enabled && !widget_->south_border_disqualified() &&
        !widget_->east_border_disqualified()) {
        this->put_global(b.south_east, south_east);
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
    Point nw{widget_->x(), widget_->y()};
    if (!b.north_west_enabled && !b.north_enabled && b.west_enabled) {
        this->put_global(b.west, nw);
    } else if (!b.north_west_enabled && !b.west_enabled && b.north_enabled) {
        this->put_global(b.north, nw);
    }
    // North-East
    Point ne{widget_->inner_x() + widget_->width() - 1 +
                 east_border_offset(*widget_),
             widget_->y()};
    if (!b.north_east_enabled && !b.north_enabled && b.east_enabled) {
        this->put_global(b.east, ne);
    } else if (!b.north_east_enabled && !b.east_enabled && b.north_enabled) {
        this->put_global(b.north, ne);
    }
    // South-West
    // Point sw{
    Point sw{widget_->x(), widget_->inner_y() + widget_->height() - 1 +
                               south_border_offset(*widget_)};
    if (!b.south_west_enabled && !b.south_enabled && b.west_enabled) {
        this->put_global(b.west, sw);
    } else if (!b.south_west_enabled && !b.west_enabled && b.south_enabled) {
        this->put_global(b.south, sw);
    }
    // South-East
    Point se{widget_->inner_x() + widget_->width() - 1 +
                 east_border_offset(*widget_),
             widget_->inner_y() + widget_->height() - 1 +
                 south_border_offset(*widget_)};
    if (!b.south_east_enabled && !b.south_enabled && b.east_enabled) {
        this->put_global(b.east, se);
    } else if (!b.south_east_enabled && !b.east_enabled && b.south_enabled) {
        this->put_global(b.south, se);
    }
}

void Painter::fill(const Glyph& tile,
                   std::size_t x,
                   std::size_t y,
                   std::size_t width,
                   std::size_t height) {
    if (width == 0) {
        return;
    }
    const std::size_t y_limit{y + height};
    for (; y < y_limit; ++y) {
        this->line(tile, x, y, width - 1, y);
    }
}

void Painter::fill(const Glyph& tile,
                   Point point,
                   std::size_t width,
                   std::size_t height) {
    this->fill(tile, point.x, point.y, width, height);
}

// Does not call down to line_global because this needs bounds checking.
void Painter::line(const Glyph& tile,
                   std::size_t x1,
                   std::size_t y1,
                   std::size_t x2,
                   std::size_t y2) {
    // Horizontal
    if (y1 == y2) {
        for (; x1 <= x2; ++x1) {
            this->put(tile, x1, y1);
        }
    }  // Vertical
    else if (x1 == x2) {
        for (; y1 <= y2; ++y1) {
            this->put(tile, x1, y1);
        }
    }
}

void Painter::line(const Glyph& tile,
                   const Point& point_1,
                   const Point& point_2) {
    this->line(tile, point_1.x, point_1.y, point_2.x, point_2.y);
}

// GLOBAL COORDINATES - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Painter::put_global(const Glyph& tile, std::size_t x, std::size_t y) {
    staged_changes_[Point{x, y}] = tile;
}

void Painter::put_global(const Glyph& tile, Point position) {
    this->put_global(tile, position.x, position.y);
}

void Painter::line_global(const Glyph& tile,
                          std::size_t x1,
                          std::size_t y1,
                          std::size_t x2,
                          std::size_t y2) {
    // Horizontal
    if (y1 == y2) {
        for (; x1 <= x2; ++x1) {
            put_global(tile, x1, y1);
        }
    }  // Vertical
    else if (x1 == x2) {
        for (; y1 <= y2; ++y1) {
            put_global(tile, x1, y1);
        }
    }
}

void Painter::line_global(const Glyph& tile,
                          const Point& point_1,
                          const Point& point_2) {
    line_global(tile, point_1.x, point_1.y, point_2.x, point_2.y);
}

}  // namespace cppurses
