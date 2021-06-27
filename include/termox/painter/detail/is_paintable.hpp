#ifndef TERMOX_PAINTER_DETAIL_IS_PAINTABLE_HPP
#define TERMOX_PAINTER_DETAIL_IS_PAINTABLE_HPP

namespace ox {
class Widget;
}  // namespace ox

namespace ox::detail {

/// A check for whether a widget is in a state that can be painted.
[[nodiscard]] auto is_paintable(Widget const& w) -> bool;

}  // namespace ox::detail
#endif  // TERMOX_PAINTER_DETAIL_IS_PAINTABLE_HPP
