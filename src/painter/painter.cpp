#include <termox/painter/painter.hpp>

#include <termox/painter/detail/is_paintable.hpp>
#include <termox/painter/detail/screen_descriptor.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/system/event_loop.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/detail/canvas.hpp>
#include <termox/widget/border.hpp>
#include <termox/widget/detail/border_offset.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace {

auto border_is_paintable(ox::Widget const& widg) -> bool
{
    return widg.border.enabled() && widg.is_enabled() &&
           widg.outer_width() != 0 && widg.outer_height() != 0;
}

}  // namespace

namespace ox {

Painter::Painter(Widget& widg, detail::Canvas& canvas)
    : widget_{widg},
      inner_area_{widget_.width(), widget_.height()},
      is_paintable_{detail::is_paintable(widget_)},
      canvas_{canvas},
      brush_{widg.brush}
{
    this->fill_global(widget_.generate_wallpaper(), widget_.top_left(),
                      widget_.outer_area());
}

void Painter::put(Glyph tile, Point p)
{
    assert(p.x < inner_area_.width && p.y < inner_area_.height);
    auto const x_global = widget_.inner_x() + p.x;
    auto const y_global = widget_.inner_y() + p.y;
    this->put_global(tile, {x_global, y_global});
}

void Painter::put(Glyph_string const& text, Point p)
{
    if (!is_paintable_)
        return;
    for (Glyph g : text)
        this->put(g, {p.x++, p.y});
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

    if (b.north.enabled() && !north_dq)
        this->hline_global(b.north, north_left, north_right);
    if (b.south.enabled() && !south_dq)
        this->hline_global(b.south, south_left, south_right);
    if (b.west.enabled() && !west_dq)
        this->vline_global(b.west, west_top, west_bottom);
    if (b.east.enabled() && !east_dq)
        this->vline_global(b.east, east_top, east_bottom);
    if (b.north_west.enabled() && !north_dq && !west_dq)
        this->put_global(b.north_west, north_west);
    if (b.north_east.enabled() && !north_dq && !east_dq)
        this->put_global(b.north_east, north_east);
    if (b.south_west.enabled() && !south_dq && !west_dq)
        this->put_global(b.south_west, south_west);
    if (b.south_east.enabled() && !south_dq && !east_dq)
        this->put_global(b.south_east, south_east);

    // Stop out of bounds drawing for special cases.
    if (north_dq && south_dq && inner_area_.height == 1)
        return;
    if (west_dq && east_dq && inner_area_.width == 1)
        return;

    // Extend Walls into Unused Corners
    // North-West
    if (!b.north_west.enabled()) {
        if (b.north.enabled() && !b.west.enabled())
            put_global(b.north, north_west);
        else if (b.west.enabled() && !b.north.enabled())
            put_global(b.west, north_west);
    }
    // North-East
    if (!b.north_east.enabled()) {
        if (b.north.enabled() && !b.east.enabled())
            put_global(b.north, north_east);
        else if (b.east.enabled() && !b.north.enabled())
            put_global(b.east, north_east);
    }
    // South-West
    if (!b.south_west.enabled()) {
        if (b.south.enabled() && !b.west.enabled())
            put_global(b.south, south_west);
        else if (b.west.enabled() && !b.south.enabled())
            put_global(b.west, south_west);
    }
    // South-East
    if (!b.south_east.enabled()) {
        if (b.south.enabled() && !b.east.enabled())
            put_global(b.south, south_east);
        else if (b.east.enabled() && !b.south.enabled())
            put_global(b.east, south_east);
    }

    // TODO might not need this anymore? with Painter constructor painting over
    // the outermost area of the widget, then the border being painted as the
    // last step in Widget::paint_event.

    // Paint wallpaper over empty space that a missing border can cause
    auto const wallpaper = widget_.generate_wallpaper();
    // North Wallpaper
    if (Offset::north(widget_) == 1 && !b.north.enabled())
        this->hline_global(wallpaper, north_left, north_right);
    // South Wallpaper
    if (Offset::south(widget_) == 1 && !b.south.enabled())
        this->hline_global(wallpaper, south_left, south_right);
    // East Wallpaper
    if (Offset::east(widget_) == 1 && !b.east.enabled())
        this->vline_global(wallpaper, east_top, east_bottom);
    // West Wallpaper
    if (Offset::west(widget_) == 1 && !b.west.enabled())
        this->vline_global(wallpaper, west_top, west_bottom);
    // North-West Wallpaper
    if (Offset::north(widget_) == 1 && Offset::west(widget_) == 1 &&
        !b.north_west.enabled())
        this->put_global(wallpaper, north_west);
    // North-East Wallpaper
    if (Offset::north(widget_) == 1 && Offset::east(widget_) == 1 &&
        !b.north_east.enabled())
        this->put_global(wallpaper, north_east);
    // South-West Wallpaper
    if (Offset::south(widget_) == 1 && Offset::west(widget_) == 1 &&
        !b.south_west.enabled())
        this->put_global(wallpaper, south_west);
    // South-East Wallpaper
    if (Offset::south(widget_) == 1 && Offset::east(widget_) == 1 &&
        !b.south_east.enabled())
        this->put_global(wallpaper, south_east);
}

void Painter::fill(Glyph tile, Point point, Area area)
{
    if (area.width == 0)
        return;
    auto const y_limit = point.y + area.height;
    auto const x_limit = point.x + area.width - 1;  // Used in inclusive context
    for (; point.y < y_limit; ++point.y)
        this->hline(tile, point, {x_limit, point.y});
}

void Painter::hline(Glyph tile, Point a, Point b)
{
    for (; a.x <= b.x; ++a.x)
        this->put(tile, a);
}

void Painter::vline(Glyph tile, Point a, Point b)
{
    for (; a.y <= b.y; ++a.y)
        this->put(tile, a);
}

// GLOBAL COORDINATES - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Painter::hline_global(Glyph tile, Point a, Point b)
{
    for (; a.x <= b.x; ++a.x)
        this->put_global(tile, a);
}

void Painter::vline_global(Glyph tile, Point a, Point b)
{
    for (; a.y <= b.y; ++a.y)
        this->put_global(tile, a);
}

void Painter::fill_global(Glyph tile, Point point, Area area)
{
    if (area.width == 0)
        return;
    auto const y_limit = point.y + area.height;
    auto const x_limit = point.x + area.width - 1;  // Used in inclusive context
    for (; point.y < y_limit; ++point.y)
        this->hline_global(tile, point, {x_limit, point.y});
}

}  // namespace ox
