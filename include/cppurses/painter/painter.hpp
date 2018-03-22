#ifndef PAINTER_PAINTER_HPP
#define PAINTER_PAINTER_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/point.hpp>

#include <cstddef>

namespace cppurses {
struct Border;
struct Glyph;
class Widget;

class Painter {
   public:
    explicit Painter(Widget* widget);

    void put(const Glyph_string& text, Point position);
    void put(const Glyph_string& text, std::size_t x, std::size_t y);
    void put(const Glyph_string& text);

    bool move_cursor_on_put{false};

    // Convinience functions
    void fill(std::size_t x,
              std::size_t y,
              std::size_t width,
              std::size_t height,
              const Glyph& tile);

    // Takes local coords
    void line(std::size_t x1,
              std::size_t y1,
              std::size_t x2,
              std::size_t y2,
              const Glyph_string& gs = "-");

    void border(const Border& b);

    void clear_screen();

   private:
    void unbound_put_string(const Point& point, const Glyph_string& gs);
    void unbound_put_string(std::size_t glob_x,
                            std::size_t glob_y,
                            const Glyph_string& gs);
    void unbound_line(const Point& point_1,
                      const Point& point_2,
                      const Glyph& symbol);
    void unbound_line(std::size_t glob_x1,
                      std::size_t glob_y1,
                      std::size_t glob_x2,
                      std::size_t glob_y2,
                      const Glyph& symbol);
    void add_default_attributes(Glyph* g);

    Widget* widget_;
};

}  // namespace cppurses
#endif  // PAINTER_PAINTER_HPP
