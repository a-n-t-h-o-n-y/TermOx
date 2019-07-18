#ifndef CPPURSES_TERMINAL_SCREEN_MUTEX_HPP
#define CPPURSES_TERMINAL_SCREEN_MUTEX_HPP
#include <mutex>

namespace cppurses {

/// Provides a mutex for ncurses memory accesses(things like resize and put).
inline auto ncurses_mutex() -> std::mutex&
{
    static std::mutex mtx;
    return mtx;
}

}  // namespace cppurses
#endif  // CPPURSES_TERMINAL_SCREEN_MUTEX_HPP
