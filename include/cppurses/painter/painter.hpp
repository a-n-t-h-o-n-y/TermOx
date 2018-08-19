#ifndef PAINTER_PAINTER_HPP
#define PAINTER_PAINTER_HPP
#include <cstddef>
#include <unordered_map>

#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph_string.hpp>  // Needed for implicit cstrs.
#include <cppurses/widget/point.hpp>

namespace cppurses {
struct Border;
struct Glyph;
class Widget;

/// Used to paint to a Widgets screen within paint_event() functions.
class Painter {
   public:
    explicit Painter(Widget* widg);
    Painter(Widget* widg, detail::Staged_changes& changes);

    /// Put single glyph to local coordinates within Widget bounds.
    void put(const Glyph& tile, std::size_t x, std::size_t y);

    /// Put single glyph to local coordinates within Widget bounds.
    void put(const Glyph& tile, Point position);

    /// Put Glyph_string to local coordinates, within Widget bounds.
    void put(const Glyph_string& text, std::size_t x, std::size_t y);

    /// Put Glyph_string to local coordinates, within Widget bounds.
    void put(const Glyph_string& text, Point position);

    /// Paint a Border object around the Widget.
    void border(const Border& b);

    /// Fill the Widget with \p tile, from top left point (x, y).
    void fill(const Glyph& tile,
              std::size_t x,
              std::size_t y,
              std::size_t width,
              std::size_t height);

    /// Fill the Widget with \p tile, from top left \p point.
    void fill(const Glyph& tile,
              Point point,
              std::size_t width,
              std::size_t height);

    /// Draws a line from x1, y1 to x2, y2 inclusive, no diagonals atm.
    void line(const Glyph& tile,
              std::size_t x1,
              std::size_t y1,
              std::size_t x2,
              std::size_t y2);

    /// Draws a line from \p point_1 to \p point_2 inclusive, no diagonals atm.
    void line(const Glyph& tile, const Point& point_1, const Point& point_2);

   private:
    /// Puts a single Glyph to the state container.
    void put_global(const Glyph& tile, std::size_t x, std::size_t y);

    /// Puts a single Glyph to the state container.
    void put_global(const Glyph& tile, Point position);

    void line_global(const Glyph& tile,
                     std::size_t x1,
                     std::size_t y1,
                     std::size_t x2,
                     std::size_t y2);

    void line_global(const Glyph& tile,
                     const Point& point_1,
                     const Point& point_2);

    // Widget you are painting to.
    Widget* widget_;

    // Reference to object held in Event_loop for this thread.
    detail::Screen_descriptor& staged_changes_;
};

}  // namespace cppurses
#endif  // PAINTER_PAINTER_HPP
