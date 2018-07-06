#ifndef PAINTER_DETAIL_PAINT_BUFFER_HPP
#define PAINTER_DETAIL_PAINT_BUFFER_HPP
#include <cstddef>
#include <mutex>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/ncurses_paint_engine.hpp>
#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/painter/palette.hpp>

namespace cppurses {
struct Glyph;

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
    detail::NCurses_paint_engine engine_;
    Glyph_matrix backing_store_;
    Glyph_matrix staging_area_;

    using Mutex_t = std::recursive_mutex;
    mutable Mutex_t mutex_;

    bool commit(std::size_t x, std::size_t y);
    void resize(std::size_t width, std::size_t height);
};

}  // namespace cppurses
#endif  // PAINTER_DETAIL_PAINT_BUFFER_HPP
