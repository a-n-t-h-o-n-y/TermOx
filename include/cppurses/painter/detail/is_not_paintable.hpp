#ifndef CPPURSES_PAINTER_DETAIL_IS_NOT_PAINTABLE_HPP
#define CPPURSES_PAINTER_DETAIL_IS_NOT_PAINTABLE_HPP
namespace cppurses {
class Widget;
namespace detail {

bool is_not_paintable(const Widget* w);

}  // namespace detail
}  // namespace cppuses
#endif  // CPPURSES_PAINTER_DETAIL_IS_NOT_PAINTABLE_HPP
