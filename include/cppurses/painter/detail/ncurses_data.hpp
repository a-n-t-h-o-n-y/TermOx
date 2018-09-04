#ifndef CPPURSES_PAINTER_DETAIL_NCURSES_DATA_HPP
#define CPPURSES_PAINTER_DETAIL_NCURSES_DATA_HPP
#include <cppurses/painter/detail/ncurses_paint_engine.hpp>

namespace cppurses {
namespace detail {

/// Shared data for ncurses calls.
class NCurses_data {
    /// Set by the WINCH signal handler.
    static bool resize_happened;

    /// Return a reference to the only NCurses_paint_engine object.
    static NCurses_paint_engine& paint_engine();

    // Only these classes and function have access to this object.
    friend class Paint_middleman;
    friend class NCurses_paint_engine;
    friend class NCurses_event_listener;
    friend void handle_sigwinch(int);
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_NCURSES_DATA_HPP
