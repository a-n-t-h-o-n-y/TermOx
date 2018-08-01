#ifndef PAINTER_DETAIL_PAINT_BUFFER_HPP
#define PAINTER_DETAIL_PAINT_BUFFER_HPP
#include <cstddef>
#include <mutex>
// #include <unordered_map>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/glyph_and_bkgd_bool.hpp>
#include <cppurses/painter/detail/ncurses_paint_engine.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
struct Glyph;
class Painter;

// TODO rename to Screen_info? or something.
class Paint_buffer {
   public:
    Paint_buffer();

    /// Applies the painted space in p to the staged diff.
    // void update_diff(const Painter& p);

    /// Apply staged_diff_ to the physical screen via engine_.
    // void flush(bool optimize);
    void flush(const detail::Staged_changes& changes);

    /// Returns the Glyph on the screen at point x, y.
    // const Glyph& at(const Point& point) const;

    /// Returns the Glyph on the screen at point x, y.
    // const Glyph& at(std::size_t x, std::size_t y) const;

    // TODO Does not need to be public?
    void move_cursor(std::size_t x, std::size_t y);

    // TODO don't need this interface, put this in screen_width directly
    std::size_t update_width();
    std::size_t update_height();

    std::size_t screen_width() const;
    std::size_t screen_height() const;

    void set_color_definition(Color c, RGB values);

    void set_global_background_tile(const Glyph& tile);
    Glyph get_global_background_tile() const;

   private:
    detail::NCurses_paint_engine engine_;

    Glyph global_background_tile_{L' '};
    std::size_t width_{0};
    std::size_t height_{0};

    using Mutex_t = std::recursive_mutex;
    mutable Mutex_t mutex_;

    void resize_width(std::size_t new_width);
    void resize_height(std::size_t new_height);

    // TODO dont need these?
    // void paint_background(Point top_left, Area area);
    // void repaint_all_backgrounds();
    void cover_with_background(Widget& w);
    bool within_screen(const Point& p);
};

}  // namespace cppurses
#endif  // PAINTER_DETAIL_PAINT_BUFFER_HPP
