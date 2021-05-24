#include <ios>
#include <termox/painter/painter.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/event_loop.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/detail/canvas.hpp>
#include <termox/widget/border.hpp>
#include <termox/widget/detail/border_offset.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace {

[[nodiscard]] auto border_is_paintable(ox::Widget const& widg) -> bool
{
    return widg.border.is_enabled() && widg.is_enabled() &&
           widg.outer_width() != 0 && widg.outer_height() != 0;
}

}  // namespace

namespace ox {

Painter::Painter(Widget& widg, detail::Canvas& canvas)
    : widget_{widg},
      inner_area_{widget_.width(), widget_.height()},
      canvas_{canvas},
      brush_{widg.brush}
{}

auto Painter::put(Glyph tile, Point p) -> Painter&
{
    // User code can contain invalid points.
    if (p.x >= inner_area_.width || p.y >= inner_area_.height || p.x < 0 ||
        p.y < 0) {
        return *this;
    }
    auto const x_global = widget_.inner_x() + p.x;
    auto const y_global = widget_.inner_y() + p.y;
    this->put_global(tile, {x_global, y_global});
    return *this;
}

auto Painter::put(Glyph_string const& text, Point p) -> Painter&
{
    for (Glyph g : text)
        this->put(g, {p.x++, p.y});
    return *this;
}

auto Painter::at(Point p) const -> Glyph
{
    auto const global = p + widget_.top_left();
    return canvas_.at(global);
}

auto Painter::at(Point p) -> Glyph&
{
    auto const global = p + widget_.top_left();
    return canvas_.at(global);
}

auto Painter::fill(Glyph tile, Point point, Area area) -> Painter&
{
    if (area.width == 0)
        return *this;
    auto const y_limit = point.y + area.height;
    auto const x_limit = point.x + area.width - 1;  // Used in inclusive context
    for (; point.y < y_limit; ++point.y)
        this->hline(tile, point, {x_limit, point.y});
    return *this;
}

auto Painter::hline(Glyph tile, Point a, Point b) -> Painter&
{
    for (; a.x <= b.x; ++a.x)
        this->put(tile, a);
    return *this;
}

auto Painter::vline(Glyph tile, Point a, Point b) -> Painter&
{
    for (; a.y <= b.y; ++a.y)
        this->put(tile, a);
    return *this;
}

auto Painter::wallpaper_fill() -> Painter&
{
    this->fill_global_no_brush(widget_.generate_wallpaper(), widget_.top_left(),
                               widget_.outer_area());
    return *this;
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

    if (b.north.is_enabled() && !north_dq)
        this->hline_global(b.north, north_left, north_right);
    if (b.south.is_enabled() && !south_dq)
        this->hline_global(b.south, south_left, south_right);
    if (b.west.is_enabled() && !west_dq)
        this->vline_global(b.west, west_top, west_bottom);
    if (b.east.is_enabled() && !east_dq)
        this->vline_global(b.east, east_top, east_bottom);
    if (b.north_west.is_enabled() && !north_dq && !west_dq)
        this->put_global(b.north_west, north_west);
    if (b.north_east.is_enabled() && !north_dq && !east_dq)
        this->put_global(b.north_east, north_east);
    if (b.south_west.is_enabled() && !south_dq && !west_dq)
        this->put_global(b.south_west, south_west);
    if (b.south_east.is_enabled() && !south_dq && !east_dq)
        this->put_global(b.south_east, south_east);

    // Stop out of bounds drawing for special cases.
    if (north_dq && south_dq && inner_area_.height == 1)
        return;
    if (west_dq && east_dq && inner_area_.width == 1)
        return;

    // Extend Walls into Unused Corners
    // North-West
    if (!b.north_west.is_enabled()) {
        if (b.north.is_enabled() && !b.west.is_enabled())
            put_global(b.north, north_west);
        else if (b.west.is_enabled() && !b.north.is_enabled())
            put_global(b.west, north_west);
    }
    // North-East
    if (!b.north_east.is_enabled()) {
        if (b.north.is_enabled() && !b.east.is_enabled())
            put_global(b.north, north_east);
        else if (b.east.is_enabled() && !b.north.is_enabled())
            put_global(b.east, north_east);
    }
    // South-West
    if (!b.south_west.is_enabled()) {
        if (b.south.is_enabled() && !b.west.is_enabled())
            put_global(b.south, south_west);
        else if (b.west.is_enabled() && !b.south.is_enabled())
            put_global(b.west, south_west);
    }
    // South-East
    if (!b.south_east.is_enabled()) {
        if (b.south.is_enabled() && !b.east.is_enabled())
            put_global(b.south, south_east);
        else if (b.east.is_enabled() && !b.south.is_enabled())
            put_global(b.east, south_east);
    }
}

// GLOBAL COORDINATES - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Painter::put_global(Glyph tile, Point p)
{
    tile.brush    = merge(tile.brush, brush_);
    canvas_.at(p) = tile;
}

void Painter::hline_global(Glyph tile, Point a, Point b)
{
    for (; a.x <= b.x; ++a.x)
        this->put_global(tile, a);
}

void Painter::hline_global_no_brush(Glyph tile, Point a, Point b)
{
    for (; a.x <= b.x; ++a.x)
        canvas_.at(a) = tile;
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

void Painter::fill_global_no_brush(Glyph tile, Point point, Area area)
{
    if (area.width == 0)
        return;
    auto const y_limit = point.y + area.height;
    auto const x_limit = point.x + area.width - 1;  // Used in inclusive context
    for (; point.y < y_limit; ++point.y)
        this->hline_global_no_brush(tile, point, {x_limit, point.y});
}

}  // namespace ox
