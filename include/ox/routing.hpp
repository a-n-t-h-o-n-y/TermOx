#pragma once

#include <optional>
#include <span>
#include <vector>

#include <ox/widget.hpp>

namespace ox {

/** A normal-tree snapshot followed by root overlays. Parent preserves logical
 * ancestry; end delimits contiguous painting subtrees (promoted children follow later).
 * Bounds are global; visibility includes every ancestor's visible state and clip.
 * Input eligibility also includes every ancestor's enabled state and the active modal
 * scope. Root overlays bypass intermediate clips, retain owner visibility, and are
 * ordered by their application-assigned overlay_order.
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
/** Replace a snapshot while retaining its allocated storage. On failure it may be
 * partial. */
void widget_tree(Widget& root, std::vector<WidgetNode>& tree);
/** The deepest hit in the last overlapping sibling subtree. */
[[nodiscard]] auto hit_test(std::span<WidgetNode const> tree, Point point)
    -> std::optional<std::size_t>;
/** Root-to-target ancestry. Later siblings take precedence in overlapping areas. */
[[nodiscard]] auto hit_path(std::span<WidgetNode const> tree, Point point)
    -> std::vector<std::size_t>;
[[nodiscard]] auto focus_order(std::span<WidgetNode const> tree)
    -> std::vector<std::size_t>;

}  // namespace ox
