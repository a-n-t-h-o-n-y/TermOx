#pragma once

#include <optional>
#include <span>
#include <vector>

#include <ox/widget.hpp>

namespace ox {

/** A depth-first snapshot. Parent and end delimit ancestry and the subtree.
 * Bounds are global; visibility includes every ancestor's active state and clip.
 * Input eligibility also includes every ancestor's enabled state.
 */
struct WidgetNode {
    LifetimeView<Widget> widget;
    std::optional<std::size_t> parent;
    std::size_t end;
    Rect bounds;
    Rect visible;
    bool input;
    auto operator==(WidgetNode const&) const -> bool = default;
};

[[nodiscard]] auto widget_tree(Widget& root) -> std::vector<WidgetNode>;
/** Root-to-target ancestry. Later siblings take precedence in overlapping areas. */
[[nodiscard]] auto hit_path(std::span<WidgetNode const> tree, Point point)
    -> std::vector<std::size_t>;
[[nodiscard]] auto focus_order(std::span<WidgetNode const> tree)
    -> std::vector<std::size_t>;

}  // namespace ox
