#ifndef PAINTER_MODULE_PAINTER_HPP
#define PAINTER_MODULE_PAINTER_HPP

#include "painter_module/color.hpp"
#include "painter_module/glyph_string.hpp"
#include "widget_module/coordinate.hpp"
#include <cstddef>

namespace twf {
class Border;
class Widget;

class Painter {
   public:
    explicit Painter(Widget* widget);

    // Essential Functions
    void put(const Glyph_string& string, bool move_cursor = true);
    void put_at(Coordinate pos,
                const Glyph_string& string,
                bool move_cursor = true);
    void put_at(std::size_t x,
                std::size_t y,
                const Glyph_string& string,
                bool move_cursor = true);

    // Moves the cursor position within Widget.
    void move(Coordinate pos, bool update_buffer = true);
    void move(std::size_t x, std::size_t y, bool update_buffer = true);

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

    void set_cursor(bool enabled);

    void clear_screen();

   private:
    void unbound_put_string(std::size_t g_x,
                            std::size_t g_y,
                            const Glyph_string& gs);
    void unbound_line(std::size_t g_x1,
                      std::size_t g_y1,
                      std::size_t g_x2,
                      std::size_t g_y2,
                      const Glyph& g);
    void add_default_attributes(Glyph* g);

    Widget* widget_;
};

}  // namespace twf
#endif  // PAINTER_MODULE_PAINTER_HPP
