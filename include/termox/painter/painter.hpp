#ifndef TERMOX_PAINTER_PAINTER_HPP
#define TERMOX_PAINTER_PAINTER_HPP
#include <termox/painter/brush.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/point.hpp>

namespace ox {
class Glyph_string;
struct Glyph;
class Widget;
}  // namespace ox

namespace ox::detail {
class Canvas;
}  // namespace ox::detail

namespace ox {

/// Contains functions to paint Glyphs to a Widget's screen area.
/** For use within Widget::paint_event(...), and virtual overrides. */
class Painter {
   public:
    /// Construct an object ready to paint Glyphs from \p w to \p canvas.
    Painter(Widget& w, detail::Canvas& canvas);

    Painter(Painter const&) = delete;
    Painter(Painter&&)      = delete;
    Painter& operator=(Painter const&) = delete;
    Painter& operator=(Painter&&) = delete;

   public:
    /// Put single Glyph to local coordinates.
    auto put(Glyph tile, Point p) -> Painter&;

    /// Put Glyph_string to local coordinates.
    auto put(Glyph_string const& text, Point p) -> Painter&;

    /// Fill the Widget with \p tile Glyphs starting at the top left \p point.
    /** \p point is in Widget local coordinates. */
    auto fill(Glyph tile, Point point, Area area) -> Painter&;

    /// Draw a horizontal line from \p a to \p b, inclusive, in local coords.
    auto hline(Glyph tile, Point a, Point b) -> Painter&;

    /// Draw a vertical line from \p a to \p b, inclusive, in local coords.
    auto vline(Glyph tile, Point a, Point b) -> Painter&;

    /// Fill the entire widget screen with wallpaper.
    auto wallpaper_fill() -> Painter&;

    /// Paint the Border object around the outside of the associated Widget.
    /** Borders own the perimeter defined by Widget::x(), Widget::y() and
     *  Widget::outer_width(), Widget::outer_height(). Border is owned by
     *  widget_ */
    void border();

   private:
    /// Put a single Glyph to the canvas_ container.
    /** No bounds checking, used internally for all painting. Main entry point
     *  for modifying the canvas_ object. */
    void put_global(Glyph tile, Point p);

    /// Paint a line from \p a to \p b inclusive using global coordinates.
    /** No bounds checking, used internally for Border object painting. */
    void hline_global(Glyph tile, Point a, Point b);

    /// Paint a line from \p a to \p b inclusive using global coordinates.
    /** No bounds checking, used internally for Border object painting. */
    void vline_global(Glyph tile, Point a, Point b);

    /// Fill the Widget with \p tile Glyphs starting at the top left \p point.
    /** No bounds checking, used internally in Painter constructor. */
    void fill_global(Glyph tile, Point point, Area area);

   private:
    Widget const& widget_;
    Area const inner_area_;
    detail::Canvas& canvas_;
    Brush brush_;
};

}  // namespace ox
#endif  // TERMOX_PAINTER_PAINTER_HPP
