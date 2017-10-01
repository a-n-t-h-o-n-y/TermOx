#ifndef PAINTER_DETAIL_NCURSES_PAINT_ENGINE_HPP
#define PAINTER_DETAIL_NCURSES_PAINT_ENGINE_HPP
#include "painter/attribute.hpp"
#include "painter/color.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

// Paint_engine Concept Requires:
// Concrete_engine(const Paint_buffer& buffer);
// void move(std::size_t x, std::size_t y);
// void show_cursor(bool show = true);
// void hide_cursor(bool hide = true);
// std::size_t screen_width();
// std::size_t screen_height();
// void clear_attributes();
// void touch_all();
// void set_ctrl_char(bool enable);
// void set_rgb(Color c, std::int16_t r, std::int16_t g, std::int16_t b);
// void put_string(const char* s);
// void put_string(const std::string& sym);
// void set_attribute(Attribute attr);
// void set_background_color(Color c);
// void set_foreground_color(Color c);
// void refresh();
// void put_glyph(const Glyph& g);
// void put(std::size_t x, std::size_t y, const Glyph& g) {

namespace cppurses {
class Paint_buffer;
class Glyph;
namespace detail {

class NCurses_paint_engine {
   public:
    explicit NCurses_paint_engine(const Paint_buffer& buffer);
    ~NCurses_paint_engine();

    NCurses_paint_engine(const NCurses_paint_engine&) = delete;
    NCurses_paint_engine(NCurses_paint_engine&&) = delete;
    NCurses_paint_engine& operator=(const NCurses_paint_engine&) = delete;
    NCurses_paint_engine& operator=(NCurses_paint_engine&&) = delete;

    void set_rgb(Color c, std::int16_t r, std::int16_t g, std::int16_t b);

    void put_glyph(const Glyph& g);
    void put(std::size_t x, std::size_t y, const Glyph& g);

    void show_cursor(bool show = true);
    void hide_cursor(bool hide = true);
    std::size_t screen_width();
    std::size_t screen_height();
    void touch_all();

    void move(std::size_t x, std::size_t y);
    void put_string(const char* s);
    void put_string(const std::string& s);

    void set_attribute(Attribute attr);
    void clear_attributes();
    void set_background_color(Color c);
    void set_foreground_color(Color c);

    void refresh();

   private:
    Color current_foreground();
    Color current_background();
    const Paint_buffer& buffer_;
};

}  // namespace detail
}  // namespace cppurses
#endif  // PAINTER_DETAIL_NCURSES_PAINT_ENGINE_HPP
