#ifndef CPPURSES_PAINTER_DETAIL_IS_NOT_PAINTABLE_HPP
#define CPPURSES_PAINTER_DETAIL_IS_NOT_PAINTABLE_HPP
namespace cppurses {
class Widget;
namespace detail {

/// Provides a check for whether a widget is in a state that can be painted.
bool is_not_paintable(const Widget* w);

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_IS_NOT_PAINTABLE_HPP
