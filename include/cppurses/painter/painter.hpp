#ifndef PAINTER_PAINTER_HPP
#define PAINTER_PAINTER_HPP
#include <cstddef>
#include <map>

#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
struct Border;
struct Glyph;
class Widget;

class Painter {
   public:
    explicit Painter(Widget* widget);

    /// Put single glyph to local coordinates within Widget bounds.
    void put(const Glyph& tile, std::size_t x, std::size_t y);

    /// Put single glyph to local coordinates within Widget bounds.
    void put(const Glyph& tile, Point position);

    /// Put Glyph_string to local coordinates, within Widget bounds.
    void put(const Glyph_string& text, std::size_t x, std::size_t y);

    /// Put Glyph_string to local coordinates, within Widget bounds.
    void put(const Glyph_string& text, Point position);

    /// Retrieve pointer to the Widget you are painting to.
    Widget* widget() const;

    /// Paint a Border object around the Widget.
    void border(const Border& b);

    void fill(const Glyph& tile,
              std::size_t x,
              std::size_t y,
              std::size_t width,
              std::size_t height);

    void fill(const Glyph& tile,
              Point point,
              std::size_t width,
              std::size_t height);

    void line(const Glyph& tile,
              std::size_t x1,
              std::size_t y1,
              std::size_t x2,
              std::size_t y2);

    void line(const Glyph& tile, const Point& point_1, const Point& point_2);

    void clear_screen();

   private:
    /// Puts a single Glyph to the state container.
    void put_global(Glyph tile, std::size_t x, std::size_t y);

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

    /// Add default attributes of Widget to the Glyph.
    Glyph add_default_attributes(const Glyph& tile);

    Widget* widget_;
    std::map<Point, Glyph> state_;
};

}  // namespace cppurses
#endif  // PAINTER_PAINTER_HPP
