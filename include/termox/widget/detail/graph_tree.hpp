#ifndef TERMOX_WIDGET_DETAIL_GRAPH_TREE_HPP
#define TERMOX_WIDGET_DETAIL_GRAPH_TREE_HPP
#include <string>

namespace ox {
class Widget;
}  // namespace ox

namespace ox::detail {

/// Outputs filename.gz graph description of widget tree hierarchy.
void graph_tree(Widget const& w, std::string const& filename);

}  // namespace ox::detail
#endif  // TERMOX_WIDGET_DETAIL_GRAPH_TREE_HPP
