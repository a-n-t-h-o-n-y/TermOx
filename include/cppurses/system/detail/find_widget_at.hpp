#ifndef CPPURSES_SYSTEM_DETAIL_FIND_WIDGET_AT_HPP
#define CPPURSES_SYSTEM_DETAIL_FIND_WIDGET_AT_HPP
#include <cppurses/widget/point.hpp>

namespace cppurses {
class Widget;
}  // namespace cppurses

namespace cppurses::detail {

/// Return the Widget* that owns the Point p in global coordinates.
/** Return nullptr on failing to find a Widget with the provided coordinates.
 *  Return the deepest child Widget that owns the coordinates. If a parent owns
 *  the coordinates, it is checked if any of the children own it as well before
 *  returning. Used only by input::get at the moment. */
auto find_widget_at(Point p) -> Widget*;

}  // namespace cppurses::detail
#endif  // CPPURSES_SYSTEM_DETAIL_FIND_WIDGET_AT_HPP
