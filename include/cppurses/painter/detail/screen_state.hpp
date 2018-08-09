#ifndef CPPURSES_PAINTER_DETAIL_SCREEN_STATE_HPP
#define CPPURSES_PAINTER_DETAIL_SCREEN_STATE_HPP
#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph.hpp>
// #include <cppurses/painter/paint_buffer.hpp>

namespace cppurses {
class Widget;
class Paint_buffer;
namespace detail {

/// Holds a Screen_descriptor representing the current screen state of a Widget.
///
/// Widget is the owner of this object, but only the flush function can modify
/// its state.
class Screen_state {
    Screen_descriptor tiles;
    Glyph background_tile;
    bool just_appeared{false};
    bool child_event_happened{false};
    bool move_happened{false};
    bool resize_happened{false};
    // friend void Paint_buffer::flush(const Staged_changes&);
    friend class cppurses::Paint_buffer;
    friend class cppurses::Widget;
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_SCREEN_STATE_HPP
