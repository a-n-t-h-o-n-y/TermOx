#ifndef CPPURSES_PAINTER_DETAIL_SCREEN_HPP
#define CPPURSES_PAINTER_DETAIL_SCREEN_HPP
#include <cppurses/painter/detail/staged_changes.hpp>

namespace cppurses::detail {

/// Writes uncommitted changes to the underlying paint engine.
/** Also enable the cursor on the widget in focus. Implements optimizations
 *  if the tile already exists onscreen. All coordinates are global. */
class Screen {
   public:
    Screen() = delete;

   public:
    /// Puts the state of \p changes onto the physical screen.
    static void flush(Staged_changes::Map_t const& changes);

    /// Moves the cursor to the currently focused widget, if cursor enabled.
    static void display_cursor();
};

}  // namespace cppurses::detail
#endif  // CPPURSES_PAINTER_DETAIL_SCREEN_HPP
