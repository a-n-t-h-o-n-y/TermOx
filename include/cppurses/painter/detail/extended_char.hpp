#ifndef CPPURSES_PAINTER_DETAIL_EXTENDED_CHAR_HPP
#define CPPURSES_PAINTER_DETAIL_EXTENDED_CHAR_HPP
#include <ncurses.h>

namespace cppurses {
namespace detail {

/// Retrieve a character that is equivalent to /p sym. For ncurses
/// implementations that do not support wide characters.
chtype find_chtype(wchar_t sym, bool* use_addch);

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_EXTENDED_CHAR_HPP
