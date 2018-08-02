#ifndef CPPURSES_SYSTEM_DETAIL_FIND_WIDGET_AT_HPP
#define CPPURSES_SYSTEM_DETAIL_FIND_WIDGET_AT_HPP
#include <cstddef>

namespace cppurses {
class Widget;
namespace detail {

Widget* find_widget_at(std::size_t x, std::size_t y);

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_FIND_WIDGET_AT_HPP
