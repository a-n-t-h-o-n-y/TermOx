#ifndef CPPURSES_PAINTER_DETAIL_IS_PAINTABLE_HPP
#define CPPURSES_PAINTER_DETAIL_IS_PAINTABLE_HPP
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {

/// A check for whether a widget is in a state that can be painted.
inline auto is_paintable(Widget const& w) -> bool
{
    return w.enabled() and (w.outer_width() != 0) and (w.outer_height() != 0);
}

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_IS_PAINTABLE_HPP
