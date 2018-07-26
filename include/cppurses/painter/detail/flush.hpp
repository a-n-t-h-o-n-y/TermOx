#ifndef CPPURSES_PAINTER_DETAIL_FLUSH_HPP
#define CPPURSES_PAINTER_DETAIL_FLUSH_HPP
#include <cppurses/painter/detail/staged_changes.hpp>

namespace cppurses {
namespace detail {

void flush(const Staged_changes& changes, bool add_default_attrs = true);

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_FLUSH_HPP
