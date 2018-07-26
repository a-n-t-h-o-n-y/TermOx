#ifndef CPPURSES_SYSTEM_DETAIL_REPAINT_ALL_HPP
#define CPPURSES_SYSTEM_DETAIL_REPAINT_ALL_HPP

namespace cppurses {
namespace detail {

/// Posts Repaint events to every widget on the tree.
void repaint_all();

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_REPAINT_ALL_HPP
