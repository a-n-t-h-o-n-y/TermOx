#ifndef PAINTER_HPP
#define PAINTER_HPP

#include "../widget_module/widget.hpp"
#include "color.hpp"
#include "glyph_string.hpp"

#include <codecvt>
#include <cstddef>
#include <locale>
#include <sstream>
#include <string>

namespace twf {
class Border;

class Painter {
   public:
    explicit Painter(Widget* widget);

    // Essential Functions
    void put(const Glyph_string& gs);
    void move(std::size_t x, std::size_t y);

    // Convinience functions
    void fill(std::size_t x,
              std::size_t y,
              std::size_t width,
              std::size_t height,
              Color fill_background);
    void line(std::size_t x1,
              std::size_t y1,
              std::size_t x2,
              std::size_t y2,
              const Glyph_string& gs = "-");
    void border(const Border& b);
    void border(const Glyph_string& gs);

    void set_cursor(bool state);

   private:
    Widget* widget_;

    void unbound_put_string(std::size_t g_x,
                            std::size_t g_y,
                            const Glyph_string& gs);
    void unbound_line(std::size_t g_x1,
                      std::size_t g_y1,
                      std::size_t g_x2,
                      std::size_t g_y2,
                      const Glyph& g);
    void add_default_attributes(Glyph& g);
};

}  // namespace twf

#endif  // PAINTER_HPP
