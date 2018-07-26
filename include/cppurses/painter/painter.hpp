#ifndef PAINTER_PAINTER_HPP
#define PAINTER_PAINTER_HPP
#include <cstddef>
#include <unordered_map>

#include <cppurses/painter/detail/glyph_and_bkgd_bool.hpp>
#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/detail/staged_changes.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
struct Border;
struct Glyph;
class Widget;

class Painter {
   public:
    explicit Painter(Widget* widg);
    Painter(Widget* widg, detail::Staged_changes& changes);

    Painter(const Painter&) = default;
    Painter(Painter&&) = default;
    Painter& operator=(const Painter&) = default;
    Painter& operator=(Painter&&) = default;

    /// Sends painted data to the paint_buffer
    // ~Painter();

    /// Put single glyph to local coordinates within Widget bounds.
    void put(const Glyph& tile, std::size_t x, std::size_t y);
    // bool is_background = false);

    /// Put single glyph to local coordinates within Widget bounds.
    void put(const Glyph& tile,
             Point position);  //, bool is_background = false);

    /// Put Glyph_string to local coordinates, within Widget bounds.
    void put(const Glyph_string& text, std::size_t x, std::size_t y);
    // bool is_background = false);

    /// Put Glyph_string to local coordinates, within Widget bounds.
    void put(const Glyph_string& text, Point position);
    // bool is_background = false);

    /// Retrieve pointer to the Widget you are painting to.
    // Widget* widget() const;  // TODO dont need?

    /// Paint a Border object around the Widget.
    void border(const Border& b);

    void fill(const Glyph& tile,
              std::size_t x,
              std::size_t y,
              std::size_t width,
              std::size_t height);
    // bool is_background = false);

    void fill(const Glyph& tile,
              Point point,
              std::size_t width,
              std::size_t height);
    // bool is_background = false);

    void line(const Glyph& tile,
              std::size_t x1,
              std::size_t y1,
              std::size_t x2,
              std::size_t y2);
    // bool is_background = false);

    void line(const Glyph& tile, const Point& point_1, const Point& point_2);
    // bool is_background = false);

    // void clear_screen();

    /// Add default attributes of Widget to the Glyph. TODO duplicate in flush
    // Glyph add_default_attributes(const Glyph& tile) const;

    // const std::unordered_map<Point, detail::Glyph_and_bkgd_bool>& state()
    // const;

   private:
    /// Puts a single Glyph to the state container.
    void put_global(Glyph tile, std::size_t x, std::size_t y);
    // bool is_background = false);

    /// Puts a single Glyph to the state container.
    void put_global(const Glyph& tile, Point position);
    // bool is_background = false);

    void line_global(const Glyph& tile,
                     std::size_t x1,
                     std::size_t y1,
                     std::size_t x2,
                     std::size_t y2);
    // bool is_background = false);

    void line_global(const Glyph& tile,
                     const Point& point_1,
                     const Point& point_2);
    // bool is_background = false);

    Widget* widget_;
    Screen_descriptor& staged_changes_;
    // std::unordered_map<Point, detail::Glyph_and_bkgd_bool> state_;
};

}  // namespace cppurses
#endif  // PAINTER_PAINTER_HPP
