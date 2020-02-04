#ifndef CPPURSES_PAINTER_DETAIL_EXTENDED_CHAR_HPP
#define CPPURSES_PAINTER_DETAIL_EXTENDED_CHAR_HPP
// This file is only included in terminal/output.cpp, so ncurses.h is fine here.
#include <ncurses.h>

namespace cppurses {
namespace detail {

/// Return a character that is equivalent to /p sym.
/** For ncurses implementations that do not support wide characters. */
auto get_chtype(wchar_t sym, bool& use_addch) -> chtype;

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_EXTENDED_CHAR_HPP
