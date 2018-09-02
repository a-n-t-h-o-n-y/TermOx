#include <cppurses/painter/detail/ncurses_data.hpp>

#include <mutex>

#include <cppurses/painter/detail/ncurses_paint_engine.hpp>

namespace cppurses {
namespace detail {

bool NCurses_data::resize_happened{false};
std::mutex NCurses_data::ncurses_mtx;

NCurses_paint_engine& NCurses_data::paint_engine() {
    static NCurses_paint_engine engine;
    return engine;
}

}  // namespace detail
}  // namespace cppurses
