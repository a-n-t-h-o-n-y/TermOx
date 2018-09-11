#ifndef CPPURSES_PAINTER_DETAIL_PAINT_MIDDLEMAN_HPP
#define CPPURSES_PAINTER_DETAIL_PAINT_MIDDLEMAN_HPP
#include <cppurses/painter/detail/ncurses_data.hpp>
#include <cppurses/painter/detail/ncurses_paint_engine.hpp>
#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>

namespace cppurses {
class Widget;
struct Point;
namespace detail {

/// Responsible for putting all staged changes to the physical screen, and
/// enabling the cursor on the widget in focus. Implementing optimizations if
/// the tile already exists onscreen.
class Paint_middleman {
   public:
    /// Puts the state of \p changes onto the physical screen.
    void flush(const detail::Staged_changes& changes);

    /// Moves the cursor to the currently focused widget, if cursor enabled.
    void set_cursor_on_focus_widget();

   private:
    NCurses_paint_engine* engine_{&NCurses_data::paint_engine()};

    // Paints wallpaper on layouts where there is no child widget.
    void paint_empty_tiles(Widget* w);

    // Covers points in w->screen_state that are not found in \p changes.
    void cover_leftovers(Widget* w, const detail::Screen_descriptor& changes);

    // Performs a full paint of a single tile at \p p.
    void full_paint_point(Widget* w,
                          const detail::Screen_descriptor& changes,
                          const Point& p);

    // Performs a basic paint of a single point at \p p.
    void basic_paint_point(Widget* w, const Point& p, Glyph tile);

    // Paints every point of \p w with wallpaper or from \p changes.
    void full_paint(Widget* w, const detail::Screen_descriptor& changes);

    // Paints tiles from \p changes, if needed, covers leftovers w/ wallpaper.
    void basic_paint(Widget* w, const detail::Screen_descriptor& changes);

    // Used when a child has just been enabled after not being on the screen.
    void paint_just_enabled(Widget* w,
                            const detail::Screen_descriptor& changes);

    // Repaints the empty space of the layout, then a basic paint of \p w.
    void paint_child_event(Widget* w, const detail::Screen_descriptor& changes);

    // Paints tiles based on a Screen_mask created by Resize_event::send();
    void paint_resize_event(Widget* w,
                            const detail::Screen_descriptor& changes);

    // TODO implement with optimizations, currently performs full repaint.
    void paint_move_event(Widget* w, const detail::Screen_descriptor& changes);
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_PAINT_MIDDLEMAN_HPP
