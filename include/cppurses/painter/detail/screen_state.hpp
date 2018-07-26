#ifndef CPPURSES_PAINTER_DETAIL_SCREEN_STATE_HPP
#define CPPURSES_PAINTER_DETAIL_SCREEN_STATE_HPP
#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>

namespace cppurses {
namespace detail {

class Screen_state {
    Screen_descriptor tiles;
    friend void flush(const Staged_changes&);
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_SCREEN_STATE_HPP
