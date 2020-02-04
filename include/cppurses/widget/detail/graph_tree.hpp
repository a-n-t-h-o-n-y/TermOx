#ifndef CPPURSES_WIDGET_DETAIL_GRAPH_TREE_HPP
#define CPPURSES_WIDGET_DETAIL_GRAPH_TREE_HPP
#include <string>

namespace cppurses {
class Widget;
namespace detail {

/// Outputs filename.gz graph description of widget tree hierarchy.
void graph_tree(Widget const& w, std::string const& filename);

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_DETAIL_GRAPH_TREE_HPP
