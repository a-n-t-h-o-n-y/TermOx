#ifndef CPPURSES_WIDGET_DETAIL_BORDER_OFFSETS_HPP
#define CPPURSES_WIDGET_DETAIL_BORDER_OFFSETS_HPP
#include <cstddef>

namespace cppurses {
class Widget;
class Painter;
namespace detail {

class Border_offset {
    static std::size_t west(const Widget& w);
    static std::size_t east(const Widget& w);
    static std::size_t north(const Widget& w);
    static std::size_t south(const Widget& w);

    static bool west_disqualified(const Widget& w);
    static bool east_disqualified(const Widget& w);
    static bool north_disqualified(const Widget& w);
    static bool south_disqualified(const Widget& w);

    friend class cppurses::Widget;
    friend class cppurses::Painter;
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_DETAIL_BORDER_OFFSETS_HPP
