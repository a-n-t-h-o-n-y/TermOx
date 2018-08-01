#ifndef PAINTER_DETAIL_PAINT_BUFFER_HPP
#define PAINTER_DETAIL_PAINT_BUFFER_HPP
#include <cstddef>
#include <mutex>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/ncurses_paint_engine.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/painter/rgb.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
struct Glyph;
class Painter;

// TODO Remove this class and place functionality elsewhere.
class Paint_buffer {
   public:
    Paint_buffer();

    /// Apply staged_diff_ to the physical screen via engine_.
    // void flush(bool optimize);
    void flush(const detail::Staged_changes& changes);

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
    // System or some global data object holds this instead. It is really only
    // used by flush? Then you could make it a static member of the flush
    // function? pretty out there stuff. Though there is probably a less
    // invisible way to do it. TODO Plug plaette will need access to
    // paint_engine. Or palette can make an event instead? but who would handle
    // it?
    detail::NCurses_paint_engine engine_;

    // TODO where to put the global background tile?
    Glyph global_background_tile_{L' '};
    std::size_t width_{0};  // TODO put these within paint engine instead
    std::size_t height_{0};

    // TODO probably dont' need these, especially if you no longer have this
    // class. Though look into if they are useful, because if they are you can't
    // just get rid of them.
    using Mutex_t = std::recursive_mutex;
    mutable Mutex_t mutex_;

    // TODO These would no longer be needed, paint_engine just retrieves the
    // size, you don't tell it its size.
    void resize_width(std::size_t new_width);
    void resize_height(std::size_t new_height);

    // TODO these are used by flush, i think, keep them around. For some reason
    // you kept flush within a class, might have been the friend declaration on
    // Widget::screen_state? or something like that. or for the mutex in
    // ncurses_data.
    void cover_with_background(Widget& w);
    bool within_screen(const Point& p);
};

}  // namespace cppurses
#endif  // PAINTER_DETAIL_PAINT_BUFFER_HPP
