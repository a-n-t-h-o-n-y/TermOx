#include <ox/routing.hpp>

#include <algorithm>

namespace ox {
namespace {
void append_tree(std::vector<WidgetNode>& tree,
                 Widget& widget,
                 std::optional<std::size_t> parent,
                 Point origin,
                 Rect clip,
                 bool input)
{
    validate(widget.size);
    auto const bounds = Rect{origin, widget.size};
    auto const visible =
        widget.active ? intersection(bounds, clip) : Rect{origin, {0, 0}};
    input = input && widget.active && widget.enabled;
    auto const index = tree.size();
    tree.push_back({track(widget), parent, 0, bounds, visible, input});
    for (auto& child : widget.get_children()) {
        append_tree(tree, child, index, translate(origin, child.at), visible, input);
    }
    tree[index].end = tree.size();
}
}  // namespace

auto widget_tree(Widget& root) -> std::vector<WidgetNode>
{
    auto tree = std::vector<WidgetNode>{};
    widget_tree(root, tree);
    return tree;
}

void widget_tree(Widget& root, std::vector<WidgetNode>& tree)
{
    tree.clear();
    append_tree(tree, root, {}, {0, 0}, {{0, 0}, root.size}, true);
}

auto hit_test(std::span<WidgetNode const> tree, Point point)
    -> std::optional<std::size_t>
{
    for (auto i = tree.size(); i > 0; --i) {
        if (tree[i - 1].input && contains(tree[i - 1].visible, point)) { return i - 1; }
    }
    return {};
}

auto hit_path(std::span<WidgetNode const> tree, Point point) -> std::vector<std::size_t>
{
    auto target = hit_test(tree, point);
    auto path = std::vector<std::size_t>{};
    while (target) {
        path.push_back(*target);
        target = tree[*target].parent;
    }
    std::ranges::reverse(path);
    return path;
}

auto focus_order(std::span<WidgetNode const> tree) -> std::vector<std::size_t>
{
    auto order = std::vector<std::size_t>{};
    for (auto i = std::size_t{0}; i < tree.size(); ++i) {
        if (!tree[i].input || !tree[i].widget.valid() ||
            tree[i].visible.size.width == 0 || tree[i].visible.size.height == 0) {
            continue;
        }
        auto const policy = tree[i].widget.get().focus_policy;
        if (policy == FocusPolicy::Tab || policy == FocusPolicy::Strong) {
            order.push_back(i);
        }
    }
    return order;
}
}  // namespace ox
