/** @file
 * Tree traversal, geometry translation, and widget hit testing.
 */
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
    /** Borrowed widget identity in a depth-first tree snapshot. */
    LifetimeView<Widget> widget;
    /** Parent node index, or nullopt for the root. */
    std::optional<std::size_t> parent;
    /** Exclusive end index of this node's depth-first subtree. */
    std::size_t end;
    /** Global unclipped widget rectangle in terminal cells. */
    Rect bounds;
    /** Global rectangle remaining after inherited clipping. */
    Rect visible;
    /** Whether this node is eligible for input after visibility, enabled, and modal rules. */
    bool input;
    /** Compare snapshot identity, ancestry, geometry, and input eligibility. */
    auto operator==(WidgetNode const&) const -> bool = default;
};

/** Build a fresh depth-first snapshot with promoted overlays, global clipping, and
 * effective input eligibility; retain no widget ownership.
 */
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
/** Return depth-first indices of visible, enabled, nonempty Tab/Strong widgets in the
 * active modal scope.
 */
[[nodiscard]] auto focus_order(std::span<WidgetNode const> tree)
    -> std::vector<std::size_t>;

}  // namespace ox
