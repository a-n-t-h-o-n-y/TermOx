#ifndef CPPURSES_PAINTER_DETAIL_STAGED_CHANGES_HPP
#define CPPURSES_PAINTER_DETAIL_STAGED_CHANGES_HPP
#include <cppurses/painter/detail/screen_descriptor.hpp>

namespace cppurses {
class Widget;
namespace detail {

/// Screen_description and whether or not it should be completely repainted.
struct Screen_info {
    bool repaint{false};
    Screen_descriptor screen_description;
};

/// Held by each Event_loop, this holds the changes to be flushed to the screen.
using Staged_changes = std::unordered_map<Widget*, Screen_info>;

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_STAGED_CHANGES_HPP
