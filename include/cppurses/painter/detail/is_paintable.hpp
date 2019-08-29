#ifndef CPPURSES_PAINTER_DETAIL_IS_PAINTABLE_HPP
#define CPPURSES_PAINTER_DETAIL_IS_PAINTABLE_HPP
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {

/// A check for whether a widget is in a state that can be painted.
inline bool is_paintable(const Widget& w) {
    return w.enabled() && (w.outer_width() != 0) && (w.outer_height() != 0);
}

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_IS_PAINTABLE_HPP
