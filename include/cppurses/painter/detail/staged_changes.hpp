#ifndef CPPURSES_PAINTER_DETAIL_STAGED_CHANGES_HPP
#define CPPURSES_PAINTER_DETAIL_STAGED_CHANGES_HPP
#include <cppurses/painter/detail/screen_descriptor.hpp>

namespace cppurses {
class Widget;
namespace detail {

/// Held by each Event_loop, this holds the changes to be flushed to the screen.
using Staged_changes = std::unordered_map<Widget*, Screen_descriptor>;

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_STAGED_CHANGES_HPP
