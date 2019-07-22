#ifndef CPPURSES_PAINTER_DETAIL_SCREEN_HPP
#define CPPURSES_PAINTER_DETAIL_SCREEN_HPP
#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>

namespace cppurses {
class Widget;
struct Point;
namespace detail {

/// Writes uncommitted changes to the underlying paint engine.
/** Also enable the cursor on the widget in focus. Implements optimizations
 *  if the tile already exists onscreen. All coordinates are global. */
class Screen {
   public:
    Screen() = delete;

    /// Puts the state of \p changes onto the physical screen.
    static void flush(const Staged_changes::Map_t& changes);

    /// Moves the cursor to the currently focused widget, if cursor enabled.
    static void set_cursor_on_focus_widget();

   private:
    /// Covers space unowned by any child widget with wallpaper.
    /** Does nothing if w has no children. */
    static void paint_empty_tiles(const Widget& widg);

    // Covers points in w->screen_state that are not found in \p staged_tiles.
    // Paints over tiles that existed on previous iteration but not on current.
    static void cover_leftovers(Widget& widg,
                                const Screen_descriptor& staged_tiles);

    // Performs a full paint of a single tile at \p point.
    // Paints either a wallpaper or staged change tile, unless the staged tile
    // is the same as what is currently on screen.
    static void full_paint_single_point(Widget& widg,
                                        const Screen_descriptor& staged_tiles,
                                        const Point& point);

    // Performs a basic paint of a single \p point.
    // Only paints if the staged change tile is different from what is onscreen.
    static void basic_paint_single_point(Widget& widg,
                                         const Point& point,
                                         Glyph tile);

    // Paint every point of \p widg with wallpaper or from \p staged_tiles.
    static void full_paint(Widget& widg, const Screen_descriptor& staged_tiles);

    // Paint \p staged_tiles(if needed), and covers leftovers w/wallpaper.
    static void basic_paint(Widget& widg,
                            const Screen_descriptor& staged_tiles);

    // Used when a child has just been enabled after not being on the screen.
    static void paint_just_enabled(Widget& widg,
                                   const Screen_descriptor& staged_tiles);

    // Repaint the empty space of the layout, then a basic paint of \p widg.
    static void paint_child_event(Widget& widg,
                                  const Screen_descriptor& staged_tiles);

    // Paint tiles based on a Screen_mask created by Resize_event::send();
    static void paint_resize_event(Widget& widg,
                                   const Screen_descriptor& staged_tiles);

    // TODO Implement with optimizations, currently performs full repaint.
    static void paint_move_event(Widget& widg,
                                 const Screen_descriptor& staged_tiles);

    // Call on the correct optimizing function to paint.
    static void delegate_paint(Widget& widg,
                               const Screen_descriptor& staged_tiles);
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_SCREEN_HPP
