#ifndef CPPURSES_PAINTER_DETAIL_IS_PAINTABLE_HPP
#define CPPURSES_PAINTER_DETAIL_IS_PAINTABLE_HPP
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {

// TODO should be protected with mutex, not this but each function that it
// calls, since these could be modified while flushing.. and this is used while
// flushing.

/// A check for whether a widget is in a state that can be painted.
inline bool is_paintable(const Widget& w) {
    return w.enabled() && (w.width() != 0) && (w.height() != 0);
}

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_IS_PAINTABLE_HPP
