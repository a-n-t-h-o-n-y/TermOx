#ifndef CPPURSES_PAINTER_DETAIL_FIND_EMPTY_SPACE_HPP
#define CPPURSES_PAINTER_DETAIL_FIND_EMPTY_SPACE_HPP
#include <cppurses/painter/detail/screen_mask.hpp>

namespace cppurses {
class Widget;
namespace detail {

/// Return a screen mask representing empty space for the Widget \p w.
/** Where a set bit means you have no enabled child owning that point. Used to
 *  find where a Layout should paint wallpaper tiles. */
Screen_mask find_empty_space(const Widget& w);

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_FIND_EMPTY_SPACE_HPP
