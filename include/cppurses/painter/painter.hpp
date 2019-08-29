#ifndef CPPURSES_PAINTER_PAINTER_HPP
#define CPPURSES_PAINTER_PAINTER_HPP
#include <cstddef>

#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/widget/area.hpp>

namespace cppurses {
class Glyph_string;
struct Point;
struct Glyph;
class Widget;

/// Contains functions to paint Glyphs to a Widget's screen area.
/** For use within Widget::paint_event(), and virtual overrides. */
class Painter {
   public:
    /// Construct an object ready to paint Glyphs to \p *widg.
    explicit Painter(Widget& widg);

    Painter(const Painter&) = delete;
    Painter(Painter&&) = delete;

    /// Put single Glyph to local coordinates, no-op if out of Widget's bounds.
    void put(const Glyph& tile, std::size_t x, std::size_t y);

    /// Put single Glyph to local coordinates, no-op if out of Widget's bounds.
    void put(const Glyph& tile, const Point& position) {
        this->put(tile, position.x, position.y);
    }

    /// Put Glyph_string to local coordinates, no-op if out of Widget's bounds
    void put(const Glyph_string& text, std::size_t x, std::size_t y);

    /// Put Glyph_string to local coordinates, no-op if out of Widget's bounds
    void put(const Glyph_string& text, const Point& position) {
        this->put(text, position.x, position.y);
    }

    /// Paint the Border object around the outside of the associated Widget.
    /** Borders own the perimeter defined by Widget::x(), Widget::y() and
     *  Widget::outer_width(), Widget::outer_height(). Border is owned by
     *  widget_ */
    void border();

    /// Fill the Widget with \p tile Glyphs, from the top left point (x, y).
    /** \p x and \p y are in Widget local coordinates. */
    void fill(const Glyph& tile,
              std::size_t x,
              std::size_t y,
              std::size_t width,
              std::size_t height);

    /// Fill the Widget with \p tile Glyphs starting at the top left \p point.
    /** \p point is in Widget local coordinates. */
    void fill(const Glyph& tile,
              const Point& point,
              std::size_t width,
              std::size_t height);

    /// Draw a straight line from [x1, y1] to [x2, y2] in local coordinates.
    /** Diagonal lines are not implemented yet. */
    void line(const Glyph& tile,
              std::size_t x1,
              std::size_t y1,
              std::size_t x2,
              std::size_t y2);

    /// Draw a straight line from \p a to \p b, inclusive, in local coordinates.
    /** Diagonal lines are not implemented yet. */
    void line(const Glyph& tile, const Point& a, const Point& b) {
        this->line(tile, a.x, a.y, b.x, b.y);
    }

   private:
    const Widget& widget_;
    const Area inner_area_;
    const bool is_paintable_;

    /// Reference to container that holds onto the painting until flush().
    /** Each Event_loop/thread has a Screen_descriptor for each Widget. */
    detail::Screen_descriptor& staged_changes_;

    /// Put a single Glyph to the staged_changes_ container.
    /** No bounds checking, used internally for all painting. Main entry point
     *  for modifying the staged_changes_ object. */
    void put_global(const Glyph& tile, std::size_t x, std::size_t y) {
        staged_changes_[{x, y}] = tile;
    }

    /// Put a single Glyph to the staged_changes_ container.
    /** No bounds checking, used internally for all painting. Main entry point
     *  for modifying the staged_changes_ object. */
    void put_global(const Glyph& tile, const Point& position) {
        this->put_global(tile, position.x, position.y);
    }

    /// Paint a line from [x1, y1] to [x2, y2] using global coordinates.
    /** No bounds checking, used internally for Border object painting. */
    void line_global(const Glyph& tile,
                     std::size_t x1,
                     std::size_t y1,
                     std::size_t x2,
                     std::size_t y2);

    /// Paint a line from \p a to \p b inclusive using global coordinates.
    /** No bounds checking, used internally for Border object painting. */
    void line_global(const Glyph& tile, const Point& a, const Point& b) {
        line_global(tile, a.x, a.y, b.x, b.y);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_PAINTER_HPP
