#ifndef CPPURSES_WIDGET_DETAIL_BORDER_OFFSETS_HPP
#define CPPURSES_WIDGET_DETAIL_BORDER_OFFSETS_HPP
#include <cstddef>

namespace cppurses {
class Widget;
class Painter;
namespace detail {

/// Provides functions to calculate distance offsets of Borders.
/** Offsets are enabled depending on the Border object itself and from the
 *  current dimensions of the passed in Widget, which may be too small to
 *  display a Border. Offsets are interpreted as the space that a border will be
 *  taking from the Widget. */
class Border_offset {
    /// Calculates west border offset, either 1 or 0.
    static std::size_t west(const Widget& w);

    /// Calculates east border offset, either 1 or 0.
    static std::size_t east(const Widget& w);

    /// Calculates north border offset, either 1 or 0.
    static std::size_t north(const Widget& w);

    /// Calculates south border offset, either 1 or 0.
    static std::size_t south(const Widget& w);

    /// Calculates whether or not \p w is too small to display the west border.
    static bool west_disqualified(const Widget& w);

    /// Calculates whether or not \p w is too small to display the east border.
    static bool east_disqualified(const Widget& w);

    /// Calculates whether or not \p w is too small to display the north border.
    static bool north_disqualified(const Widget& w);

    /// Calculates whether or not \p w is too small to display the south border.
    static bool south_disqualified(const Widget& w);

    friend class cppurses::Widget;
    friend class cppurses::Painter;
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_DETAIL_BORDER_OFFSETS_HPP
