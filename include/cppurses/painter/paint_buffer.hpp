#ifndef PAINTER_DETAIL_PAINT_BUFFER_HPP
#define PAINTER_DETAIL_PAINT_BUFFER_HPP
#include "painter/color.hpp"
#include "painter/glyph_matrix.hpp"
#include "painter/detail/ncurses_paint_engine.hpp"
#include "painter/palette.hpp"
#include <cstddef>

namespace cppurses {
class Glyph;

class Paint_buffer {
   public:
    Paint_buffer();

    void stage(std::size_t x, std::size_t y, const Glyph& glyph);
    void flush(bool optimize);
    void move(std::size_t x, std::size_t y);

    std::size_t update_width();
    std::size_t update_height();

    void set_color(Color c, RGB values);
    const Glyph& at(std::size_t x, std::size_t y) const;

   private:
    detail::NCurses_paint_engine engine_{*this};
    Glyph_matrix backing_store_;
    Glyph_matrix staging_area_;

    bool commit(std::size_t x, std::size_t y);
    void resize(std::size_t width, std::size_t height);
};

}  // namespace cppurses
#endif  // PAINTER_DETAIL_PAINT_BUFFER_HPP
