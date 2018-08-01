#ifndef CPPURSES_PAINTER_DETAIL_NCURSES_DATA_HPP
#define CPPURSES_PAINTER_DETAIL_NCURSES_DATA_HPP
#include <mutex>

namespace cppurses {
class Paint_buffer;
namespace detail {

/// Shared data for ncurses calls.
class NCurses_data {
    /// Set by the WINCH signal handler.
    static bool resize_happened;

    /// Locked in functions that modify ncurses internal state.
    static std::mutex ncurses_mtx;

    // Only these classes and function have access to this object.
    friend class cppurses::Paint_buffer;
    friend class NCurses_paint_engine;
    friend class NCurses_event_listener;
    friend void handle_sigwinch(int);
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_NCURSES_DATA_HPP
