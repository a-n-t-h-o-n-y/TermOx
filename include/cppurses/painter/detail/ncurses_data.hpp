#ifndef CPPURSES_PAINTER_DETAIL_NCURSES_DATA_HPP
#define CPPURSES_PAINTER_DETAIL_NCURSES_DATA_HPP
#include <mutex>

namespace cppurses {
class Paint_buffer;
namespace detail {

class NCurses_data {
    static bool resize_happened;
    static std::mutex ncurses_mtx;

    friend class cppurses::Paint_buffer;
    friend class NCurses_paint_engine;
    friend class NCurses_event_listener;
    friend void handle_sigwinch(int);
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_NCURSES_DATA_HPP
