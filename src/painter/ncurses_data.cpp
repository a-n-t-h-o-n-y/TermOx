#include <cppurses/painter/detail/ncurses_data.hpp>

#include <cppurses/painter/detail/ncurses_paint_engine.hpp>

namespace cppurses {
namespace detail {

bool NCurses_data::resize_happened{false};

NCurses_paint_engine& NCurses_data::paint_engine() {
    static NCurses_paint_engine engine;
    return engine;
}

void NCurses_data::initialize() {
    paint_engine();
}

}  // namespace detail
}  // namespace cppurses
