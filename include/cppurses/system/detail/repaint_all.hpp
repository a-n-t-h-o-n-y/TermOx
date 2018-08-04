#ifndef CPPURSES_SYSTEM_DETAIL_REPAINT_ALL_HPP
#define CPPURSES_SYSTEM_DETAIL_REPAINT_ALL_HPP

namespace cppurses {
class Widget;
namespace detail {

/// Posts Repaint events to every widget on the tree.
void repaint_all();

/// Posts Repaint_events to \p w and all of this children, recursively.
void post_repaint_recursive(Widget* w);

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_REPAINT_ALL_HPP
