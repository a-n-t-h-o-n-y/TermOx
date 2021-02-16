#ifndef TERMOX_PAINTER_DETAIL_IS_PAINTABLE_HPP
#define TERMOX_PAINTER_DETAIL_IS_PAINTABLE_HPP
#include <termox/widget/widget.hpp>

namespace ox::detail {

/// A check for whether a widget is in a state that can be painted.
inline auto is_paintable(Widget const& w) -> bool
{
    return w.is_enabled() && w.type_can_paint() && (w.outer_width() != 0) &&
           (w.outer_height() != 0);
}

}  // namespace ox::detail
#endif  // TERMOX_PAINTER_DETAIL_IS_PAINTABLE_HPP
