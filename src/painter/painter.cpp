#include <cppurses/painter/painter.hpp>

#include <cstddef>
#include <string>
#include <unordered_map>

#include <cppurses/painter/detail/is_paintable.hpp>
#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/event_loop.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/detail/border_offset.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;

bool border_is_paintable(const Widget& widg) {
    return widg.border.enabled && widg.enabled() && widg.outer_width() != 0 &&
           widg.outer_height() != 0;
}

}  // namespace

namespace cppurses {

Painter::Painter(Widget& widg)
    : widget_{widg},
      staged_changes_{System::find_event_loop().staged_changes()[&widg]} {}

void Painter::put(const Glyph& tile, std::size_t x, std::size_t y) {
    if (x >= widget_.width() || y >= widget_.height()) {
        return;
    }
    const auto x_global = widget_.inner_x() + x;
    const auto y_global = widget_.inner_y() + y;
    this->put_global(tile, x_global, y_global);
}

void Painter::put(const Glyph_string& text, std::size_t x, std::size_t y) {
    if (!detail::is_paintable(widget_)) {
        return;
    }
    for (const Glyph& g : text) {
        this->put(g, x++, y);
    }
}

void Painter::border() {
    if (!border_is_paintable(widget_)) {
        return;
    }
    // Disqualified borders
    using detail::Border_offset;
    const bool west_disqualified = Border_offset::west_disqualified(widget_);
    const bool east_disqualified = Border_offset::east_disqualified(widget_);
    const bool north_disqualified = Border_offset::north_disqualified(widget_);
    const bool south_disqualified = Border_offset::south_disqualified(widget_);

    // North Wall
    const auto north_left = Point{widget_.inner_x(), widget_.y()};
    const auto north_right =
        Point{north_left.x + widget_.width() - 1, north_left.y};

    // South Wall
    const auto south_left =
        Point{north_left.x, widget_.y() + widget_.outer_height() - 1};
    const auto south_right = Point{north_right.x, south_left.y};

    // West Wall
    const auto west_top = Point{widget_.x(), widget_.inner_y()};
    const auto west_bottom =
        Point{west_top.x, west_top.y + widget_.height() - 1};

    // East Wall
    const auto east_top =
        Point{west_top.x + widget_.outer_width() - 1, west_top.y};
    const auto east_bottom = Point{east_top.x, west_bottom.y};

    // Corners
    const auto north_east = Point{east_top.x, north_left.y};
    const auto north_west = Point{west_top.x, north_right.y};
    const auto south_east = Point{east_bottom.x, south_left.y};
    const auto south_west = Point{west_bottom.x, south_right.y};

    const auto& b = widget_.border;

    // General Border Construction/Painting.
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
    if (widget_.height() == 1 && north_disqualified && south_disqualified) {
        return;
    }
    if (widget_.width() == 1 && west_disqualified && east_disqualified) {
        return;
    }

    // Extend Walls into Unused Corners
    // North-West
    if (!b.north_west_enabled) {
        if (b.north_enabled && !b.west_enabled) {
            put_global(b.north, north_west);
        } else if (b.west_enabled && !b.north_enabled) {
            put_global(b.west, north_west);
        }
    }
    // North-East
    if (!b.north_east_enabled) {
        if (b.north_enabled && !b.east_enabled) {
            put_global(b.north, north_east);
        } else if (b.east_enabled && !b.north_enabled) {
            put_global(b.east, north_east);
        }
    }
    // South-West
    if (!b.south_west_enabled) {
        if (b.south_enabled && !b.west_enabled) {
            put_global(b.south, south_west);
        } else if (b.west_enabled && !b.south_enabled) {
            put_global(b.west, south_west);
        }
    }
    // South-East
    if (!b.south_east_enabled) {
        if (b.south_enabled && !b.east_enabled) {
            put_global(b.south, south_east);
        } else if (b.east_enabled && !b.south_enabled) {
            put_global(b.east, south_east);
        }
    }

    // Paint wallpaper over empty space that a missing border can cause
    // North Wallpaper
    const auto wallpaper = widget_.generate_wallpaper();
    if (Border_offset::north(widget_) == 1 && !b.north_enabled) {
        this->line_global(wallpaper, north_left, north_right);
    }

    // South Wallpaper
    if (Border_offset::south(widget_) == 1 && !b.south_enabled) {
        this->line_global(wallpaper, south_left, south_right);
    }

    // East Wallpaper
    if (Border_offset::east(widget_) == 1 && !b.east_enabled) {
        this->line_global(wallpaper, east_top, east_bottom);
    }

    // West Wallpaper
    if (Border_offset::west(widget_) == 1 && !b.west_enabled) {
        this->line_global(wallpaper, west_top, west_bottom);
    }

    // North-West Wallpaper
    if (Border_offset::north(widget_) == 1 &&
        Border_offset::west(widget_) == 1 && !b.north_west_enabled) {
        this->put_global(wallpaper, north_west);
    }

    // North-East Wallpaper
    if (Border_offset::north(widget_) == 1 &&
        Border_offset::east(widget_) == 1 && !b.north_east_enabled) {
        this->put_global(wallpaper, north_east);
    }

    // South-West Wallpaper
    if (Border_offset::south(widget_) == 1 &&
        Border_offset::west(widget_) == 1 && !b.south_west_enabled) {
        this->put_global(wallpaper, south_west);
    }

    // South-East Wallpaper
    if (Border_offset::south(widget_) == 1 &&
        Border_offset::east(widget_) == 1 && !b.south_east_enabled) {
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
    const auto y_limit = y + height;
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

// GLOBAL COORDINATES - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

}  // namespace cppurses
