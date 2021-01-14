#include <termox/painter/painter.hpp>

#include <cstddef>
#include <string>
#include <unordered_map>

#include <termox/painter/detail/is_paintable.hpp>
#include <termox/painter/detail/screen_descriptor.hpp>
#include <termox/painter/detail/staged_changes.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/system/event_loop.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/border.hpp>
#include <termox/widget/detail/border_offset.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace {

auto border_is_paintable(ox::Widget const& widg) -> bool
{
    return widg.border.enabled() and widg.is_enabled() and
           widg.outer_width() != 0 and widg.outer_height() != 0;
}

}  // namespace

namespace ox {

Painter::Painter(Widget& widg)
    : widget_{widg},
      inner_area_{widget_.width(), widget_.height()},
      is_paintable_{detail::is_paintable(widget_)},
      staged_changes_{detail::Staged_changes::get()[&widg]}
{}

void Painter::put(Glyph tile, std::size_t x, std::size_t y)
{
    if (x >= inner_area_.width or y >= inner_area_.height)
        return;
    auto const x_global = widget_.inner_x() + x;
    auto const y_global = widget_.inner_y() + y;
    this->put_global(tile, x_global, y_global);
}

void Painter::put(Glyph_string const& text, std::size_t x, std::size_t y)
{
    if (!is_paintable_)
        return;
    for (Glyph g : text)
        this->put(g, x++, y);
}

void Painter::border()
{
    using Offset = detail::Border_offset;
    if (!border_is_paintable(widget_))
        return;
    // Disqualified borders
    auto const west_dq  = Offset::west_disqualified(widget_);
    auto const east_dq  = Offset::east_disqualified(widget_);
    auto const north_dq = Offset::north_disqualified(widget_);
    auto const south_dq = Offset::south_disqualified(widget_);

    // North Wall
    auto const north_left = Point{widget_.inner_x(), widget_.y()};
    auto const north_right =
        Point{north_left.x + inner_area_.width - 1, north_left.y};

    // South Wall
    auto const south_left =
        Point{north_left.x, widget_.y() + widget_.outer_height() - 1};
    auto const south_right = Point{north_right.x, south_left.y};

    // West Wall
    auto const west_top = Point{widget_.x(), widget_.inner_y()};
    auto const west_bottom =
        Point{west_top.x, west_top.y + inner_area_.height - 1};

    // East Wall
    auto const east_top =
        Point{west_top.x + widget_.outer_width() - 1, west_top.y};
    auto const east_bottom = Point{east_top.x, west_bottom.y};

    // Corners
    auto const north_east = Point{east_top.x, north_left.y};
    auto const north_west = Point{west_top.x, north_right.y};
    auto const south_east = Point{east_bottom.x, south_left.y};
    auto const south_west = Point{west_bottom.x, south_right.y};

    // General Border Construction/Painting
    auto const& b = widget_.border.segments;

    if (b.north.enabled() and !north_dq)
        this->line_global(b.north, north_left, north_right);
    if (b.south.enabled() and !south_dq)
        this->line_global(b.south, south_left, south_right);
    if (b.west.enabled() and !west_dq)
        this->line_global(b.west, west_top, west_bottom);
    if (b.east.enabled() and !east_dq)
        this->line_global(b.east, east_top, east_bottom);
    if (b.north_west.enabled() and !north_dq and !west_dq)
        this->put_global(b.north_west, north_west);
    if (b.north_east.enabled() and !north_dq and !east_dq)
        this->put_global(b.north_east, north_east);
    if (b.south_west.enabled() and !south_dq and !west_dq)
        this->put_global(b.south_west, south_west);
    if (b.south_east.enabled() and !south_dq and !east_dq)
        this->put_global(b.south_east, south_east);

    // Stop out of bounds drawing for special cases.
    if (north_dq and south_dq and inner_area_.height == 1)
        return;
    if (west_dq and east_dq and inner_area_.width == 1)
        return;

    // Extend Walls into Unused Corners
    // North-West
    if (!b.north_west.enabled()) {
        if (b.north.enabled() and !b.west.enabled())
            put_global(b.north, north_west);
        else if (b.west.enabled() and !b.north.enabled())
            put_global(b.west, north_west);
    }
    // North-East
    if (!b.north_east.enabled()) {
        if (b.north.enabled() and !b.east.enabled())
            put_global(b.north, north_east);
        else if (b.east.enabled() and !b.north.enabled())
            put_global(b.east, north_east);
    }
    // South-West
    if (!b.south_west.enabled()) {
        if (b.south.enabled() and !b.west.enabled())
            put_global(b.south, south_west);
        else if (b.west.enabled() and !b.south.enabled())
            put_global(b.west, south_west);
    }
    // South-East
    if (!b.south_east.enabled()) {
        if (b.south.enabled() and !b.east.enabled())
            put_global(b.south, south_east);
        else if (b.east.enabled() and !b.south.enabled())
            put_global(b.east, south_east);
    }

    // Paint wallpaper over empty space that a missing border can cause
    auto const wallpaper = widget_.generate_wallpaper();
    // North Wallpaper
    if (Offset::north(widget_) == 1 and !b.north.enabled())
        this->line_global(wallpaper, north_left, north_right);
    // South Wallpaper
    if (Offset::south(widget_) == 1 and !b.south.enabled())
        this->line_global(wallpaper, south_left, south_right);
    // East Wallpaper
    if (Offset::east(widget_) == 1 and !b.east.enabled())
        this->line_global(wallpaper, east_top, east_bottom);
    // West Wallpaper
    if (Offset::west(widget_) == 1 and !b.west.enabled())
        this->line_global(wallpaper, west_top, west_bottom);
    // North-West Wallpaper
    if (Offset::north(widget_) == 1 and Offset::west(widget_) == 1 and
        !b.north_west.enabled())
        this->put_global(wallpaper, north_west);
    // North-East Wallpaper
    if (Offset::north(widget_) == 1 and Offset::east(widget_) == 1 and
        !b.north_east.enabled())
        this->put_global(wallpaper, north_east);
    // South-West Wallpaper
    if (Offset::south(widget_) == 1 and Offset::west(widget_) == 1 and
        !b.south_west.enabled())
        this->put_global(wallpaper, south_west);
    // South-East Wallpaper
    if (Offset::south(widget_) == 1 and Offset::east(widget_) == 1 and
        !b.south_east.enabled())
        this->put_global(wallpaper, south_east);
}

void Painter::fill(Glyph tile,
                   std::size_t x,
                   std::size_t y,
                   std::size_t width,
                   std::size_t height)
{
    if (width == 0)
        return;
    auto const y_limit = y + height;
    for (; y < y_limit; ++y)
        this->line(tile, x, y, width - 1, y);
}

void Painter::fill(Glyph tile, Point const& point, Area const& area)
{
    this->fill(tile, point.x, point.y, area.width, area.height);
}

// Does not call down to line_global because this needs bounds checking.
void Painter::line(Glyph tile,
                   std::size_t x1,
                   std::size_t y1,
                   std::size_t x2,
                   std::size_t y2)
{
    // Horizontal
    if (y1 == y2) {
        for (; x1 <= x2; ++x1)
            this->put(tile, x1, y1);
    }  // Vertical
    else if (x1 == x2) {
        for (; y1 <= y2; ++y1)
            this->put(tile, x1, y1);
    }
}

// GLOBAL COORDINATES - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Painter::line_global(Glyph tile,
                          std::size_t x1,
                          std::size_t y1,
                          std::size_t x2,
                          std::size_t y2)
{
    // Horizontal
    if (y1 == y2) {
        for (; x1 <= x2; ++x1)
            this->put_global(tile, x1, y1);
    }  // Vertical
    else if (x1 == x2) {
        for (; y1 <= y2; ++y1)
            this->put_global(tile, x1, y1);
    }
}

}  // namespace ox
