#ifndef TERMOX_PAINTER_DETAIL_FIND_EMPTY_SPACE_HPP
#define TERMOX_PAINTER_DETAIL_FIND_EMPTY_SPACE_HPP
#include <termox/painter/detail/screen_mask.hpp>

namespace ox {
class Widget;
}  // namespace ox

namespace ox::detail {

/// Return a screen mask representing empty space for the Widget \p w.
/** Where a set bit means you have no enabled child owning that point. Used to
 *  find where a Layout should paint wallpaper tiles. */
auto find_empty_space(Widget const& w) -> Screen_mask;

}  // namespace ox::detail
#endif  // TERMOX_PAINTER_DETAIL_FIND_EMPTY_SPACE_HPP
