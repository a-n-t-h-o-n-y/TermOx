#include <ox/routing.hpp>

#include <algorithm>

namespace ox {
namespace {
void append_tree(
    std::vector<WidgetNode>& tree,
    Widget& widget,
    std::optional<std::size_t> parent,
    Point origin,
    Rect clip,
    bool input,
    std::vector<std::tuple<LifetimeView<Widget>, bool, bool, std::size_t>>& overlays)
{
    validate(widget.size());
    auto const bounds = Rect{origin, widget.size()};
    auto const visible =
        widget.visible ? intersection(bounds, clip) : Rect{origin, {0, 0}};
    input = input && widget.visible && widget.enabled;
    auto const index = tree.size();
    tree.push_back({track(widget), parent, 0, bounds, visible, input});
    for (auto& child : widget.get_children()) {
        if (child.overlay) {
            overlays.push_back({track(child), input,
                                visible.size.width > 0 && visible.size.height > 0,
                                index});
        }
        else {
            append_tree(tree, child, index, translate(origin, child.at()), visible,
                        input, overlays);
        }
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
    auto overlays =
        std::vector<std::tuple<LifetimeView<Widget>, bool, bool, std::size_t>>{};
    auto const clip = Rect{{}, root.size()};
    append_tree(tree, root, {}, {}, clip, true, overlays);
    for (std::size_t i = 0; i < overlays.size(); ++i) {
        std::stable_sort(overlays.begin() + (std::ptrdiff_t)i, overlays.end(),
                         [](auto const& a, auto const& b) {
                             auto* x = std::get<0>(a).try_get();
                             auto* y = std::get<0>(b).try_get();
                             return (x ? x->overlay_order : 0) <
                                    (y ? y->overlay_order : 0);
                         });
        auto const [target, input, show, parent] = overlays[i];
        if (auto* widget = target.try_get()) {
            append_tree(tree, *widget, parent, widget->at(), show ? clip : Rect{},
                        input, overlays);
        }
    }
    tree[0].end = tree.size();
    auto active = std::optional<std::size_t>{};
    for (std::size_t i = 0; i < tree.size(); ++i) {
        auto const& n = tree[i];
        if (n.input && n.visible.size.width && n.visible.size.height &&
            n.widget.valid() && n.widget.get().modal) {
            active = i;
        }
    }
    if (active) {
        for (std::size_t i = 0; i < tree.size(); ++i) {
            auto ancestor = std::optional<std::size_t>{i};
            while (ancestor && ancestor != active) {
                ancestor = tree[*ancestor].parent;
            }
            if (!ancestor) { tree[i].input = false; }
        }
    }
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
