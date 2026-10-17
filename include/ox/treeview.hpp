/** @file
 * Expandable trees, node paths, and visible-row selection.
 */
#pragma once
#include <ox/listview.hpp>
namespace ox {
/** Owned tree label, eligibility, expansion state, children, and row styling. */
struct TreeNode {
    /** Printable single-line UTF-8 node label. */
    std::string text;
    /** Whether the node can be selected or activated. */
    bool enabled = true;
    /** Whether this tree branch initially exposes its children. */
    bool expanded = false;
    /** Owned immediate child nodes; paths index these vectors from the root model. */
    std::vector<TreeNode> children = {};
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    StylePatch style = {};
};
/** Paths are ordinary child-index vectors; replacing the model clears selection. */
class TreeView : public Widget {
   public:
    using Widget::handle;
    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Owned root nodes of the initial tree model. */
        std::vector<TreeNode> nodes = {};
        /** Enable multiple visible-node selection through the embedded list. */
        bool multiple = false;
        /** Presentation options for the tree's visible-row list. */
        ListView::Options list = {};
        /** Draw tree branch connectors and continuation lines. */
        bool show_guides = true;
        /** Style patch for tree connectors; selection backgrounds are preserved. */
        StylePatch guide_style = {};
    };
    /** Draw tree branch connectors and continuation lines. */
    bool show_guides;
    /** Style patch for tree connectors; selection backgrounds are preserved. */
    StylePatch guide_style;
    /** Emits selected root-to-node index paths when visible selection changes. */
    sl::Signal<void(std::vector<std::vector<std::size_t>> const&)> on_select;
    /** Emits the activated node's root-to-node index path. */
    sl::Signal<void(std::vector<std::size_t> const&)> on_activate;
    /** Emits the changed node path and its new expanded state. */
    sl::Signal<void(std::vector<std::size_t> const&, bool)> on_expand;
    /** Create an empty single-selection tree with branch guides. */
    TreeView();
    /** Validate and own the root model and visible-list presentation options. */
    explicit TreeView(Options);
    /** Borrow root nodes until set_nodes() or a model mutation. */
    auto nodes() const -> std::span<TreeNode const> { return nodes_; }
    /** Inspect the embedded visible-row list without bypassing path-based mutation commands. */
    auto list() const -> ListView const& { return list_; }
    /** Copy selected visible-node paths, each expressed as root-to-node child indices. */
    auto selected_paths() const -> std::vector<std::vector<std::size_t>>;
    /** Return the current visible navigation path, or nullopt. */
    auto current_path() const -> std::optional<std::vector<std::size_t>>;
    /** Validate and replace the model, clear selection, and rebuild visible rows;
     * request layout/repaint.
     */
    void set_nodes(std::vector<TreeNode>);
    /** Change a valid node's expansion, rebuild visible rows, preserve surviving paths,
     * and emit on_expand on change.
     */
    auto set_expanded(std::vector<std::size_t> const&, bool) -> bool;
    /** Expand ancestors of an existing path, then select it through the visible list;
     * invalid paths throw and disabled nodes remain unselected.
     */
    auto select(std::vector<std::size_t> const&,
                SelectionAction = SelectionAction::Replace) -> bool;
    /** Return the embedded view's analyzed content extent in terminal cells. */
    auto content_size() const -> Area { return list_.content_size(); }
    /** Return the embedded view's visible terminal-cell extent. */
    auto viewport_size() const -> Area { return list_.viewport_size(); }
    /** Return the embedded view's clamped content offset in columns and rows. */
    auto scroll_offset() const -> Point { return list_.scroll_offset(); }
    /** Forward scrolling to the embedded view, which clamps and emits on_scroll only for
     * effective state changes.
     */
    auto set_scroll_offset(Point at) -> bool { return list_.set_scroll_offset(at); }
    /** Emits (offset, content extent, viewport extent) when scroll state or geometry
     * changes; units are terminal columns and rows.
     */
    sl::Signal<void(Point, Area, Area)> on_scroll;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    void paint_overlay(Canvas) override;
    auto get_children() -> zzz::Generator<Widget&> override { co_yield list_; }
    auto get_children() const -> zzz::Generator<Widget const&> override
    { co_yield list_; }
    auto handle(KeyEvent const&) -> EventResponse override;

   protected:
    void on_arrange(Rect) override;

   private:
    std::vector<TreeNode> nodes_;
    ListView list_;
    std::vector<std::vector<std::size_t>> paths_;
    bool rebuilding_ = false;
    std::uint64_t model_revision_ = 0;
    std::uint64_t rebuild_revision_ = 0;
    auto node(std::vector<std::size_t> const&) -> TreeNode&;
    auto rebuild(std::optional<std::vector<std::size_t>> current = {},
                 bool preserve = true) -> bool;
    void activate(std::size_t);
};
}  // namespace ox
