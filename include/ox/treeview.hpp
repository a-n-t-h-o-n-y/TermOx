#pragma once
#include <ox/listview.hpp>
namespace ox {
struct TreeNode {
    std::string text;
    bool enabled = true;
    bool expanded = false;
    std::vector<TreeNode> children = {};
};
/** Paths are ordinary child-index vectors; replacing the model clears selection. */
class TreeView : public Widget {
   public:
    using Widget::handle;
    struct Options {
        std::vector<TreeNode> nodes = {};
        bool multiple = false;
        ListView::Options list = {};
    };
    sl::Signal<void(std::vector<std::vector<std::size_t>> const&)> on_select;
    sl::Signal<void(std::vector<std::size_t> const&)> on_activate;
    sl::Signal<void(std::vector<std::size_t> const&, bool)> on_expand;
    TreeView();
    explicit TreeView(Options);
    auto nodes() const -> std::span<TreeNode const> { return nodes_; }
    auto list() const -> ListView const& { return list_; }
    auto selected_paths() const -> std::vector<std::vector<std::size_t>>;
    auto current_path() const -> std::optional<std::vector<std::size_t>>;
    void set_nodes(std::vector<TreeNode>);
    auto set_expanded(std::vector<std::size_t> const&, bool) -> bool;
    auto select(std::vector<std::size_t> const&,
                SelectionAction = SelectionAction::Replace) -> bool;
    auto content_size() const -> Area { return list_.content_size(); }
    auto viewport_size() const -> Area { return list_.viewport_size(); }
    auto scroll_offset() const -> Point { return list_.scroll_offset(); }
    auto set_scroll_offset(Point at) -> bool { return list_.set_scroll_offset(at); }
    sl::Signal<void(Point, Area, Area)> on_scroll;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
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
    auto node(std::vector<std::size_t> const&) -> TreeNode&;
    void rebuild(std::optional<std::vector<std::size_t>> current = {},
                 bool preserve = true);
    void activate(std::size_t);
};
}  // namespace ox
