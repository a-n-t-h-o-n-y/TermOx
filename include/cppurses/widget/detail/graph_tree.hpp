#ifndef CPPURSES_WIDGET_DETAIL_GRAPH_TREE_HPP
#define CPPURSES_WIDGET_DETAIL_GRAPH_TREE_HPP
#include <string>

namespace cppurses {
class Widget;
namespace detail {

void graph_tree(const Widget& w, const std::string& filename);

}
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_DETAIL_GRAPH_TREE_HPP
