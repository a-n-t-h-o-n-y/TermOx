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
    static void flush(Staged_changes::Map_t const& changes);

    /// Moves the cursor to the currently focused widget, if cursor enabled.
    static void set_cursor_on_focus_widget();

   private:
    static void paint_empty_tiles(Widget const& widg, Glyph const& wallpaper);

    static void full_paint(Widget& widg, Screen_descriptor const& staged_tiles);
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_SCREEN_HPP
