#ifndef PAINTER_DETAIL_NCURSES_PAINT_ENGINE_HPP
#define PAINTER_DETAIL_NCURSES_PAINT_ENGINE_HPP
#include <cstddef>
#include <cstdint>

#include <cppurses/painter/color.hpp>

/* Paint_engine Requires:
 *   void put_glyph(const Glyph& g);
 *   void put(std::size_t x, std::size_t y, const Glyph& g) {
 *   void move_cursor(std::size_t x, std::size_t y);
 *   void show_cursor(bool show = true);
 *   void hide_cursor(bool hide = true);
 *   std::size_t screen_width();
 *   std::size_t screen_height();
 *   void touch_all();
 *   void refresh();
 *   void set_rgb(Color c, std::int16_t r, std::int16_t g, std::int16_t b);
 */

namespace cppurses {
struct Glyph;
namespace detail {

class NCurses_paint_engine {
   public:
    explicit NCurses_paint_engine();
    ~NCurses_paint_engine();

    NCurses_paint_engine(const NCurses_paint_engine&) = delete;
    NCurses_paint_engine(NCurses_paint_engine&&) = default;
    NCurses_paint_engine& operator=(const NCurses_paint_engine&) = delete;
    NCurses_paint_engine& operator=(NCurses_paint_engine&&) = default;

    void put_glyph(const Glyph& g);
    void put(std::size_t x, std::size_t y, const Glyph& g);
    void move_cursor(std::size_t x, std::size_t y);

    void show_cursor(bool show = true);
    void hide_cursor(bool hide = true);
    std::size_t screen_width();
    std::size_t screen_height();

    void touch_all();
    void refresh();

    void set_rgb(Color c, std::int16_t r, std::int16_t g, std::int16_t b);
};

}  // namespace detail
}  // namespace cppurses
#endif  // PAINTER_DETAIL_NCURSES_PAINT_ENGINE_HPP
