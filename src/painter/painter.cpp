#include <termox/painter/painter.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/event_loop.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/detail/canvas.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

Painter::Painter(Widget& widg, detail::Canvas& canvas)
    : widget_{widg}, canvas_{canvas}, brush_{widg.brush}
{
    this->wallpaper_fill();
}

auto Painter::put(Glyph tile, Point p) -> Painter&
{
    // User code can contain invalid points.
    if (p.x >= widget_.area().width || p.y >= widget_.area().height ||
        p.x < 0 || p.y < 0) {
        return *this;
    }
    this->put_global(tile, widget_.top_left() + p);
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
                               widget_.area());
    return *this;
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
