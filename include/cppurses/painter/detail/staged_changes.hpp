#ifndef CPPURSES_PAINTER_DETAIL_STAGED_CHANGES_HPP
#define CPPURSES_PAINTER_DETAIL_STAGED_CHANGES_HPP
#include <cppurses/painter/detail/screen_descriptor.hpp>

namespace cppurses {
class Widget;
namespace detail {

struct Screen_info {
    bool repaint{false};
    Screen_descriptor screen_description;
};

/// Each Widget has its own Screen_descriptor for staged changes. Used by
/// Painter to stage changes to specific widgets.
using Staged_changes = std::unordered_map<Widget*, Screen_info>;

// using Staged_changes = std::unordered_map<Widget*, Screen_descriptor>;

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_STAGED_CHANGES_HPP
