#include <ox/treeview.hpp>
namespace ox {
namespace {
void validate_nodes(std::vector<TreeNode> const& nodes)
{
    for (auto const& n : nodes) {
        validate_row_text(n.text);
        validate_nodes(n.children);
    }
}
}  // namespace
TreeView::TreeView() : TreeView{Options{}} {}
TreeView::TreeView(Options x)
    : Widget{}, list_{[&] {
          x.list.multiple = x.multiple;
          return std::move(x.list);
      }()}
{
    Connection{.signal = list_.on_select, .slot = [](auto const&, TreeView& self) {
                   if (!self.rebuilding_) {
                       auto selected = self.selected_paths();
                       self.on_select(selected);
                   }
               }}(*this);
    Connection{
        .signal = list_.on_activate,
        .slot = [](std::size_t row, TreeView& self) { self.activate(row); }}(*this);
    Connection{.signal = list_.on_scroll,
        .slot = [](Point at, Area content, Area viewport, TreeView& self) {
            self.on_scroll(at, content, viewport);
        }}(*this);
    set_nodes(std::move(x.nodes));
}
auto TreeView::node(std::vector<std::size_t> const& path) -> TreeNode&
{
    if (path.empty()) throw std::invalid_argument{"Empty tree path"};
    auto* nodes = &nodes_;
    TreeNode* result = nullptr;
    for (auto i : path) {
        if (i >= nodes->size()) throw std::out_of_range{"Tree path"};
        result = &(*nodes)[i];
        nodes = &result->children;
    }
    return *result;
}
void TreeView::set_nodes(std::vector<TreeNode> nodes)
{
    validate_nodes(nodes);
    auto previous = selected_paths();
    nodes_ = std::move(nodes);
    auto lifetime = track(*this);
    rebuild({}, false);
    if (auto* self = lifetime.try_get(); self && !previous.empty()) {
        auto empty = std::vector<std::vector<std::size_t>>{};
        self->on_select(empty);
    }
}
auto TreeView::selected_paths() const -> std::vector<std::vector<std::size_t>>
{
    auto result = std::vector<std::vector<std::size_t>>{};
    for (auto i : list_.selected_rows())
        if (i < paths_.size()) result.push_back(paths_[i]);
    return result;
}
auto TreeView::current_path() const -> std::optional<std::vector<std::size_t>>
{
    auto row = list_.current_row();
    return row && *row < paths_.size() ? std::optional{paths_[*row]} : std::nullopt;
}
void TreeView::rebuild(std::optional<std::vector<std::size_t>> current, bool preserve)
{
    auto selected =
        preserve ? selected_paths() : std::vector<std::vector<std::size_t>>{};
    auto items = std::vector<ListItem>{};
    auto paths = decltype(paths_){};
    auto path = std::vector<std::size_t>{};
    auto visit = [&](auto& recurse, std::vector<TreeNode> const& nodes,
                     bool enabled) -> void {
        for (std::size_t i = 0; i < nodes.size(); ++i) {
            auto const& n = nodes[i];
            path.push_back(i);
            paths.push_back(path);
            items.push_back({std::string((path.size() - 1) * 2, ' ') +
                                 (n.children.empty() ? "  "
                                  : n.expanded       ? "▾ "
                                                     : "▸ ") +
                                 n.text,
                             enabled && n.enabled});
            if (n.expanded) recurse(recurse, n.children, enabled && n.enabled);
            path.pop_back();
        }
    };
    visit(visit, nodes_, true);
    paths_ = std::move(paths);
    rebuilding_ = true;
    auto lifetime = track(*this);
    list_.set_items(std::move(items));
    auto* self = lifetime.try_get();
    if (!self) return;
    for (auto const& p : selected) {
        auto it = std::find(self->paths_.begin(), self->paths_.end(), p);
        if (it != self->paths_.end())
            self->list_.select((std::size_t)(it - self->paths_.begin()),
                               self->list_.multiple ? SelectionAction::Toggle
                                                    : SelectionAction::Replace);
        self = lifetime.try_get();
        if (!self) return;
    }
    if (current) {
        auto it = std::find(self->paths_.begin(), self->paths_.end(), *current);
        if (it != self->paths_.end())
            self->list_.select((std::size_t)(it - self->paths_.begin()),
                               SelectionAction::Move);
    }
    self = lifetime.try_get();
    if (!self) return;
    self->rebuilding_ = false;
    self->refresh();
}
auto TreeView::set_expanded(std::vector<std::size_t> const& path, bool expanded) -> bool
{
    auto copy = path;
    auto& n = node(copy);
    if (n.children.empty() || n.expanded == expanded) return false;
    auto before = selected_paths();
    auto current = current_path();
    n.expanded = expanded;
    if (!expanded && current && current->size() > copy.size() &&
        std::equal(copy.begin(), copy.end(), current->begin()))
        current = copy;
    auto lifetime = track(*this);
    rebuild(current);
    auto* self = lifetime.try_get();
    if (!self) return true;
    auto after = self->selected_paths();
    if (before != after) self->on_select(after);
    if ((self = lifetime.try_get())) self->on_expand(copy, expanded);
    return true;
}
auto TreeView::select(std::vector<std::size_t> const& path, SelectionAction action)
    -> bool
{
    node(path);
    auto prefix = std::vector<std::size_t>{};
    auto lifetime = track(*this);
    for (std::size_t i = 0; i + 1 < path.size(); ++i) {
        prefix.push_back(path[i]);
        auto* self = lifetime.try_get();
        if (!self) return false;
        self->set_expanded(prefix, true);
    }
    auto* self = lifetime.try_get();
    if (!self) return false;
    auto it = std::find(self->paths_.begin(), self->paths_.end(), path);
    return it != self->paths_.end() &&
           self->list_.select((std::size_t)(it - self->paths_.begin()), action);
}
void TreeView::activate(std::size_t row)
{
    if (row >= paths_.size()) return;
    auto path = paths_[row];
    auto lifetime = track(*this);
    auto& n = node(path);
    if (!n.children.empty()) set_expanded(path, !n.expanded);
    if (auto* self = lifetime.try_get()) self->on_activate(path);
}
auto TreeView::measure(Area a, esc::WidthPolicy p) const -> Area
{ return list_.measure(a, p); }
void TreeView::on_arrange(Rect) { arrange(list_, {{}, size()}); }
auto TreeView::handle(KeyEvent const& e) -> EventResponse
{
    if (!is_enabled() || e.action == KeyAction::Release || !e.key ||
        e.modifiers != Modifiers{})
        return {};
    auto path = current_path();
    if (!path) return {};
    auto& n = node(*path);
    if (e.matches(KeyCode::ArrowRight, {}, e.action)) {
        if (!n.children.empty() && !n.expanded)
            return {.handled = true, .repaint = set_expanded(*path, true)};
        if (!n.children.empty()) {
            path->push_back(0);
            return {.handled = true, .repaint = select(*path)};
        }
        return {.handled = true};
    }
    if (e.matches(KeyCode::ArrowLeft, {}, e.action)) {
        if (n.expanded) return {.handled = true, .repaint = set_expanded(*path, false)};
        if (path->size() > 1) {
            path->pop_back();
            return {.handled = true, .repaint = select(*path)};
        }
        return {.handled = true};
    }
    return {};
}
}  // namespace ox
