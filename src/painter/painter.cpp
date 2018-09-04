#include <cppurses/painter/painter.hpp>

#include <cstddef>
#include <string>
#include <unordered_map>

#include <cppurses/painter/detail/is_not_paintable.hpp>
#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/event_loop.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/detail/border_offset.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Painter::Painter(Widget* widg)
    : widget_{widg},
      staged_changes_{System::find_event_loop().staged_changes()[widg]} {}

void Painter::put(const Glyph& tile, std::size_t x, std::size_t y) {
    if (x >= widget_->width() || y >= widget_->height()) {
        return;
    }
    std::size_t glob_x = widget_->inner_x() + x;
    std::size_t glob_y = widget_->inner_y() + y;
    this->put_global(tile, glob_x, glob_y);
}

void Painter::put(const Glyph& tile, const Point& position) {
    this->put(tile, position.x, position.y);
}

void Painter::put(const Glyph_string& text, std::size_t x, std::size_t y) {
    if (detail::is_not_paintable(widget_)) {
        return;
    }
    for (const Glyph& g : text) {
        this->put(g, x++, y);
    }
}

void Painter::put(const Glyph_string& text, const Point& position) {
    this->put(text, position.x, position.y);
}

void Painter::border(const Border& b) {
    if (!b.enabled || !widget_->enabled() || widget_->outer_width() == 0 ||
        widget_->outer_height() == 0) {
        return;
    }

    // Disqualified borders
    bool west_disqualified{detail::Border_offset::west_disqualified(*widget_)};
    bool east_disqualified{detail::Border_offset::east_disqualified(*widget_)};
    bool north_disqualified{
        detail::Border_offset::north_disqualified(*widget_)};
    bool south_disqualified{
        detail::Border_offset::south_disqualified(*widget_)};

    // North Wall
    Point north_left{widget_->inner_x(), widget_->y()};
    Point north_right{north_left.x + widget_->width() - 1, north_left.y};

    // South Wall
    Point south_left{north_left.x, widget_->y() + widget_->outer_height() - 1};
    Point south_right{north_right.x, south_left.y};

    // West Wall
    Point west_top{widget_->x(), widget_->inner_y()};
    Point west_bottom{west_top.x, west_top.y + widget_->height() - 1};

    // East Wall
    Point east_top{west_top.x + widget_->outer_width() - 1, west_top.y};
    Point east_bottom{east_top.x, west_bottom.y};

    // Corners
    Point north_east{east_top.x, north_left.y};
    Point north_west{west_top.x, north_right.y};
    Point south_east{east_bottom.x, south_left.y};
    Point south_west{west_bottom.x, south_right.y};

    // North
    if (b.north_enabled && !north_disqualified) {
        this->line_global(b.north, north_left, north_right);
    }
    // South
    if (b.south_enabled && !south_disqualified) {
        this->line_global(b.south, south_left, south_right);
    }
    // West
    if (b.west_enabled && !west_disqualified) {
        this->line_global(b.west, west_top, west_bottom);
    }
    // East
    if (b.east_enabled && !east_disqualified) {
        this->line_global(b.east, east_top, east_bottom);
    }
    // North-West
    if (b.north_west_enabled && !north_disqualified && !west_disqualified) {
        this->put_global(b.north_west, north_west);
    }
    // North-East
    if (b.north_east_enabled && !north_disqualified && !east_disqualified) {
        this->put_global(b.north_east, north_east);
    }
    // South-West
    if (b.south_west_enabled && !south_disqualified && !west_disqualified) {
        this->put_global(b.south_west, south_west);
    }
    // South-East
    if (b.south_east_enabled && !south_disqualified && !east_disqualified) {
        this->put_global(b.south_east, south_east);
    }

    // Stop out of bounds drawing for special cases.
    if (widget_->height() == 1 && north_disqualified && south_disqualified) {
        return;
    }
    if (widget_->width() == 1 && west_disqualified && east_disqualified) {
        return;
    }

    // Corners - Special Case Glyph Change
    // North-West
    if (!b.north_west_enabled && !b.north_enabled && b.west_enabled) {
        this->put_global(b.west, north_west);
    } else if (!b.north_west_enabled && !b.west_enabled && b.north_enabled) {
        this->put_global(b.north, north_west);
    }
    // North-East
    if (!b.north_east_enabled && !b.north_enabled && b.east_enabled) {
        this->put_global(b.east, north_east);
    } else if (!b.north_east_enabled && !b.east_enabled && b.north_enabled) {
        this->put_global(b.north, north_east);
    }
    // South-West
    if (!b.south_west_enabled && !b.south_enabled && b.west_enabled) {
        this->put_global(b.west, south_west);
    } else if (!b.south_west_enabled && !b.west_enabled && b.south_enabled) {
        this->put_global(b.south, south_west);
    }
    // South-East
    if (!b.south_east_enabled && !b.south_enabled && b.east_enabled) {
        this->put_global(b.east, south_east);
    } else if (!b.south_east_enabled && !b.east_enabled && b.south_enabled) {
        this->put_global(b.south, south_east);
    }

    // Paint wallpaper over empty space that border causes.
    // North Wallpaper
    Glyph wallpaper{widget_->generate_wallpaper()};
    if (detail::Border_offset::north(*widget_) == 1 && !b.north_enabled) {
        this->line_global(wallpaper, north_left, north_right);
    }

    // South Wallpaper
    if (detail::Border_offset::south(*widget_) == 1 && !b.south_enabled) {
        this->line_global(wallpaper, south_left, south_right);
    }

    // East Wallpaper
    if (detail::Border_offset::east(*widget_) == 1 && !b.east_enabled) {
        this->line_global(wallpaper, east_top, east_bottom);
    }

    // West Wallpaper
    if (detail::Border_offset::west(*widget_) == 1 && !b.west_enabled) {
        this->line_global(wallpaper, west_top, west_bottom);
    }

    // North-West Wallpaper
    if (detail::Border_offset::north(*widget_) == 1 &&
        detail::Border_offset::west(*widget_) == 1 && !b.north_west_enabled) {
        this->put_global(wallpaper, north_west);
    }

    // North-East Wallpaper
    if (detail::Border_offset::north(*widget_) == 1 &&
        detail::Border_offset::east(*widget_) == 1 && !b.north_east_enabled) {
        this->put_global(wallpaper, north_east);
    }

    // South-West Wallpaper
    if (detail::Border_offset::south(*widget_) == 1 &&
        detail::Border_offset::west(*widget_) == 1 && !b.south_west_enabled) {
        this->put_global(wallpaper, south_west);
    }

    // South-East Wallpaper
    if (detail::Border_offset::south(*widget_) == 1 &&
        detail::Border_offset::east(*widget_) == 1 && !b.south_east_enabled) {
        this->put_global(wallpaper, south_east);
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
                   const Point& point,
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

void Painter::line(const Glyph& tile, const Point& a, const Point& b) {
    this->line(tile, a.x, a.y, b.x, b.y);
}

// GLOBAL COORDINATES - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Painter::put_global(const Glyph& tile, std::size_t x, std::size_t y) {
    staged_changes_[Point{x, y}] = tile;
}

void Painter::put_global(const Glyph& tile, const Point& position) {
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
            this->put_global(tile, x1, y1);
        }
    }  // Vertical
    else if (x1 == x2) {
        for (; y1 <= y2; ++y1) {
            this->put_global(tile, x1, y1);
        }
    }
}

void Painter::line_global(const Glyph& tile, const Point& a, const Point& b) {
    line_global(tile, a.x, a.y, b.x, b.y);
}

}  // namespace cppurses
