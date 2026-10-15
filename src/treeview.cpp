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
    ++model_revision_;
    auto lifetime = track(*this);
    if (!rebuild({}, false)) { return; }
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
auto TreeView::rebuild(std::optional<std::vector<std::size_t>> current, bool preserve)
    -> bool
{
    auto revision = ++rebuild_revision_;
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
    auto owner = [lifetime, revision]() -> TreeView* {
        auto* self = lifetime.try_get();
        return self && self->rebuild_revision_ == revision ? self : nullptr;
    };
    try {
        list_.set_items(std::move(items));
        auto* self = owner();
        if (!self) { return false; }
        for (auto const& p : selected) {
            auto it = std::find(self->paths_.begin(), self->paths_.end(), p);
            if (it != self->paths_.end()) {
                self->list_.select((std::size_t)(it - self->paths_.begin()),
                                   self->list_.multiple ? SelectionAction::Toggle
                                                        : SelectionAction::Replace);
            }
            self = owner();
            if (!self) { return false; }
        }
        if (current) {
            auto it = std::find(self->paths_.begin(), self->paths_.end(), *current);
            if (it != self->paths_.end()) {
                self->list_.select((std::size_t)(it - self->paths_.begin()),
                                   SelectionAction::Move);
            }
        }
        self = owner();
        if (!self) { return false; }
        self->rebuilding_ = false;
        self->refresh();
        return owner() != nullptr;
    }
    catch (...) {
        if (auto* self = owner()) { self->rebuilding_ = false; }
        throw;
    }
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
    if (!rebuild(current)) { return true; }
    auto* self = lifetime.try_get();
    if (!self) return true;
    auto revision = self->rebuild_revision_;
    auto after = self->selected_paths();
    if (before != after) self->on_select(after);
    if ((self = lifetime.try_get()) && self->rebuild_revision_ == revision) {
        self->on_expand(copy, expanded);
    }
    return true;
}
auto TreeView::select(std::vector<std::size_t> const& path, SelectionAction action)
    -> bool
{
    auto requested = path;
    node(requested);
    auto revision = model_revision_;
    auto prefix = std::vector<std::size_t>{};
    auto lifetime = track(*this);
    for (std::size_t i = 0; i + 1 < requested.size(); ++i) {
        prefix.push_back(requested[i]);
        auto* self = lifetime.try_get();
        if (!self || self->model_revision_ != revision) { return false; }
        self->set_expanded(prefix, true);
    }
    auto* self = lifetime.try_get();
    if (!self || self->model_revision_ != revision) { return false; }
    auto it = std::find(self->paths_.begin(), self->paths_.end(), requested);
    return it != self->paths_.end() &&
           self->list_.select((std::size_t)(it - self->paths_.begin()), action);
}

void TreeView::activate(std::size_t row)
{
    if (row >= paths_.size()) return;
    auto path = paths_[row];
    auto revision = model_revision_;
    auto lifetime = track(*this);
    auto& n = node(path);
    if (!n.children.empty()) set_expanded(path, !n.expanded);
    if (auto* self = lifetime.try_get(); self && self->model_revision_ == revision) {
        self->on_activate(path);
    }
}
void TreeView::paint_overlay(Canvas canvas)
{
    if (!theme().styled) return;
    auto offset = list_.scroll_offset();
    auto clip = canvas.visible();
    auto first = std::max<std::int64_t>(0, (std::int64_t)offset.y + clip.at.y);
    auto last =
        std::min<std::int64_t>((std::int64_t)paths_.size(), first + clip.size.height);
    for (auto row = first; row < last; ++row) {
        auto const& path = paths_[(std::size_t)row];
        auto const* siblings = &nodes_;
        auto depth = path.size() - 1;
        auto selected =
            std::ranges::binary_search(list_.selected_rows(), (std::size_t)row);
        auto write = [&](std::int64_t x, char32_t glyph, bool connector = true) {
            x += 2 - offset.x;
            auto y = (int)(row - offset.y);
            if (x < 2 || x >= canvas.size().width || !canvas.contains({(int)x, y}))
                return;
            auto at = Point{(int)x, y};
            auto ink = canvas[at].style;
            if (connector && !selected &&
                list_.style.foreground == Color{default_color})
                ink.foreground = theme().palette.border;
            canvas.write(at, std::u32string_view{&glyph, 1}, ink);
        };
        for (std::size_t level = 0; level < path.size(); ++level) {
            auto index = path[level];
            auto const& node = (*siblings)[index];
            auto last_sibling = index + 1 == siblings->size();
            if (level > 0) {
                auto x = 2 * (std::int64_t)(level - 1);
                if (level < depth) {
                    if (!last_sibling) write(x, theme().ascii ? U'|' : U'│');
                }
                else {
                    write(x, theme().ascii ? (last_sibling ? U'`' : U'+')
                                           : (last_sibling ? U'└' : U'├'));
                    write(x + 1, theme().ascii ? U'-' : U'─');
                    if (node.children.empty())
                        write(x + 2, theme().ascii ? U'-' : U'╴');
                }
            }
            if (level == depth && !node.children.empty() && theme().ascii)
                write(2 * (std::int64_t)depth, node.expanded ? U'v' : U'>', false);
            siblings = &node.children;
        }
    }
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
