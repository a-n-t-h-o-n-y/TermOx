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
    // void put(const Glyph_string& string, bool move_cursor = true);

    void put(const Glyph_string& string,
             std::size_t x = 0,
             std::size_t y = 0,
             bool move_cursor = false);
    void put(const Glyph_string& string,
             Coordinate position,
             bool move_cursor = false);

    // void put_at(Coordinate pos,
    //             const Glyph_string& string,
    //             bool move_cursor = true);

    // void put_at(std::size_t x,
    //             std::size_t y,
    //             const Glyph_string& string,
    //             bool move_cursor = true);

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
    void unbound_put_string(const Glyph_string& gs,
                            std::size_t glob_x,
                            std::size_t glob_y);
    void unbound_line(std::size_t glob_x1,
                      std::size_t glob_y1,
                      std::size_t glob_x2,
                      std::size_t glob_y2,
                      const Glyph& symbol);
    void add_default_attributes(Glyph* g);

    Widget* widget_;
};

}  // namespace twf
#endif  // PAINTER_MODULE_PAINTER_HPP
