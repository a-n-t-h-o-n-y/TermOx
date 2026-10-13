#include <ox/application.hpp>

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace ox {
namespace {
// Only delivery ancestry survives callbacks, not the entire tree. Indices allow
// constant-time eligibility checks when a callback leaves the structure intact.
auto ancestry(std::span<WidgetNode const> tree, std::optional<std::size_t> target)
    -> std::vector<std::pair<std::size_t, WidgetNode>>
{
    auto path = std::vector<std::pair<std::size_t, WidgetNode>>{};
    while (target) {
        path.emplace_back(*target, tree[*target]);
        target = tree[*target].parent;
    }
    return path;
}

auto ancestry(std::span<WidgetNode const> tree, LifetimeView<Widget> target)
    -> std::vector<std::pair<std::size_t, WidgetNode>>
{
    auto const found = std::ranges::find(tree, target, &WidgetNode::widget);
    return ancestry(tree, found == tree.end() ? std::optional<std::size_t>{}
                                              : (std::size_t)(found - tree.begin()));
}

auto accepts_input(std::span<WidgetNode const> tree,
                   std::size_t index,
                   LifetimeView<Widget> target) -> bool
{
    if (!target.valid()) { return false; }
    if (index < tree.size() && tree[index].widget == target) {
        return tree[index].input;
    }
    auto const found = std::ranges::find(tree, target, &WidgetNode::widget);
    return found != tree.end() && found->input;
}

void paint_tree(std::span<WidgetNode const> tree,
                std::size_t index,
                Canvas canvas,
                LifetimeView<Widget> focus,
                Cursor& cursor)
{
    auto const& node = tree[index];
    if (!node.widget.valid() || node.visible.size.width == 0 ||
        node.visible.size.height == 0) {
        return;
    }
    for (auto child = index + 1; child < node.end; child = tree[child].end) {
        auto const& bounds = tree[child].bounds;
        auto const offset =
            Point{bounds.at.x - node.bounds.at.x, bounds.at.y - node.bounds.at.y};
        paint_tree(tree, child, canvas.subcanvas(offset, bounds.size), focus, cursor);
    }
    if (!node.widget.valid()) { return; }
    auto& widget = node.widget.get();
    widget.paint(canvas);
    if (node.widget.valid() && node.widget == focus) {
        auto const local = node.widget.get().cursor;
        cursor = local && canvas.contains(*local) ? Cursor{canvas.to_global(*local)}
                                                  : Cursor{};
    }
}
}  // namespace

Application::Application(Widget& head) : head_{head}, focus_{head, loop_}
{
    try {
        tree_ = widget_tree(head_);
        attach_widgets();
        attachments_current_ = true;
        focus_.reconcile(tree_);
    }
    catch (...) {
        for (auto const& target : attached_) {
            if (target.valid() && target.get().application_ == this) {
                for (auto const& timer : target.get().timers_) {
                    loop_.unwatch(timer);
                }
                target.get().focused = false;
                target.get().application_ = nullptr;
            }
        }
        throw;
    }
}

Application::~Application()
{
    try {
        focus_.clear();
    }
    catch (...) {
    }  // Cleanup must continue even if a user callback throws.
    for (auto const& target : attached_) {
        if (target.valid() && target.get().application_ == this) {
            for (auto const& timer : target.get().timers_) {
                loop_.unwatch(timer);
            }
            target.get().focused = false;
            target.get().application_ = nullptr;
        }
    }
}

void Application::attach_widgets()
{
    auto current = std::move(attachment_storage_);
    current.clear();
    current.reserve(tree_.size());
    auto complete = true;
    for (auto const& node : tree_) {
        if (!node.widget.valid()) { continue; }
        auto& widget = node.widget.get();
        if (widget.application_ && widget.application_ != this) {
            throw std::logic_error{"Widget already belongs to an application"};
        }
        complete &= widget.application_ == this;
        current.push_back(node.widget);
    }
    if (complete && current == attached_) {
        attachment_storage_ = std::move(current);
        return;
    }
    auto present = std::vector<Widget*>{};
    present.reserve(current.size());
    for (auto const& target : current) {
        present.push_back(&target.get());
    }
    std::ranges::sort(present, std::less<Widget*>{});
    for (auto const& target : attached_) {
        if (!target.valid() ||
            std::ranges::binary_search(present, &target.get(), std::less<Widget*>{})) {
            continue;
        }
        auto& widget = target.get();
        widget.application_ = nullptr;
        for (auto const& entry : widget.timers_) {
            loop_.unwatch(entry);
        }
    }
    attached_.swap(current);
    attachment_storage_ = std::move(current);
    for (auto const& target : attached_) {
        auto& widget = target.get();
        if (widget.application_ == this) { continue; }
        for (auto const& timer : widget.timers_) {
            loop_.watch(timer);
        }
        widget.application_ = this;
        std::erase_if(widget.timers_,
                      [](auto const& timer) { return timer.expired(); });
    }
}

void Application::report(EventResponse response)
{
    if (response.repaint) { loop_.request_repaint(); }
    if (response.quit && !loop_.exit_code()) { loop_.quit(*response.quit); }
}

auto Application::is_interactive(Widget const& widget) const -> bool
{
    auto const tree = widget_tree(head_);
    return std::ranges::any_of(tree, [&](auto const& node) {
        return node.input && node.widget.valid() && &node.widget.get() == &widget;
    });
}

auto Application::update_hover() -> std::pair<EventResponse, bool>
{
    auto const target = mouse_position_ ? hit_test(tree_, *mouse_position_)
                                        : std::optional<std::size_t>{};
    auto index = target;
    auto count = hovered_.size();
    while (index && count > 0 && tree_[*index].widget == hovered_[count - 1]) {
        index = tree_[*index].parent;
        --count;
    }
    if (!index && count == 0) { return {{}, false}; }
    auto next = std::vector<LifetimeView<Widget>>{};
    index = target;
    while (index) {
        next.push_back(tree_[*index].widget);
        index = tree_[*index].parent;
    }
    std::ranges::reverse(next);
    auto common = std::size_t{0};
    while (common < hovered_.size() && common < next.size() &&
           hovered_[common] == next[common]) {
        ++common;
    }
    auto old = std::exchange(hovered_, std::move(next));
    auto entering = hovered_;  // Callbacks may mutate or reenter the tree.
    auto result = EventResponse{};
    for (auto i = old.size(); i > common; --i) {
        if (old[i - 1].valid()) {
            result = combine(result, old[i - 1].get().mouse_leave());
            if (result.quit) { return {result, true}; }
        }
    }
    for (auto i = common; i < entering.size(); ++i) {
        if (entering[i].valid() && is_interactive(entering[i].get())) {
            result = combine(result, entering[i].get().mouse_enter());
            if (result.quit) { break; }
        }
    }
    return {result, true};
}

auto Application::refresh_tree() -> bool
{
    // Keep traversal storage local: get_children() can reenter the application.
    auto next = std::move(tree_storage_);
    widget_tree(head_, next);
    auto const changed = next != tree_;
    tree_.swap(next);
    tree_storage_ = std::move(next);
    if (changed || !attachments_current_) {
        attachments_current_ = false;
        attach_widgets();
        attachments_current_ = true;
    }
    return changed;
}

auto Application::synchronize() -> EventResponse
{
    if (auto code = loop_.exit_code()) { return {.quit = code}; }
    auto result = EventResponse{.repaint = refresh_tree()};
    // Focus consumes the snapshot before invoking callbacks; it never retains it.
    auto const focus_response = focus_.reconcile(tree_);
    result = combine(result, focus_response);
    if (focus_response.repaint || focus_response.handled || focus_response.quit) {
        refresh_tree();
    }
    if (!result.quit) {
        auto const [response, changed] = update_hover();
        result = combine(result, response);
        if (!result.quit && changed) {
            result.repaint |= refresh_tree();
            auto const focus_response = focus_.reconcile(tree_);
            result = combine(result, focus_response);
            if (focus_response.repaint || focus_response.handled ||
                focus_response.quit) {
                refresh_tree();
            }
        }
    }
    report(result);
    return result;
}

auto Application::run(Terminal::Options options) -> int
{
    if (auto code = loop_.exit_code()) { return *code; }
    auto terminal = Terminal{options};
    return process_events(loop_, terminal, *this, [this] { synchronize(); });
}

auto Application::step(EventLoop::Clock::time_point now) -> EventResponse
{
    auto result = synchronize();
    if (!result.quit) { result = combine(result, loop_.step(*this, now)); }
    if (!result.quit) { result = combine(result, synchronize()); }
    return result;
}

auto Application::paint() -> bool
{
    synchronize();
    return loop_.paint(*this);
}
void Application::post(Event event) { loop_.post(std::move(event)); }
void Application::request_repaint() { loop_.request_repaint(); }
void Application::quit(int code) { loop_.quit(code); }

template <typename Input>
auto Application::route_mouse(Input event,
                              EventResponse (Widget::*callback)(Input),
                              bool select) -> EventResponse
{
    mouse_position_ = event.at;
    auto result = synchronize();
    auto const path = ancestry(tree_, hit_test(tree_, event.at));
    auto focus_changed = false;
    if (select && !result.quit) {
        for (auto const& [index, node] : path) {
            auto const target = node.widget;
            if (!accepts_input(tree_, index, target)) { continue; }
            auto const policy = target.get().focus_policy;
            if (policy == FocusPolicy::Click || policy == FocusPolicy::Strong) {
                auto const response = focus_.set(target.get());
                focus_changed =
                    response.repaint || response.handled || response.quit.has_value();
                result = combine(result, response);
                break;
            }
        }
    }
    if (focus_changed && !result.quit) { result = combine(result, synchronize()); }
    // Focus/hover effects do not consume the input event itself.
    auto input = EventResponse{};
    for (auto const& [index, node] : path) {
        if (result.quit) { break; }
        if (!accepts_input(tree_, index, node.widget)) { continue; }
        auto local = event;
        local.at = {event.at.x - node.bounds.at.x, event.at.y - node.bounds.at.y};
        input = combine(input, (node.widget.get().*callback)(local));
        if (!input.quit) { result = combine(result, synchronize()); }
        if (input.handled || input.quit) { break; }
    }
    result = combine(result, input);
    report(result);
    return result;
}

template <typename Callback>
auto Application::route_focused(Callback callback) -> EventResponse
{
    auto result = synchronize();
    auto const path = ancestry(tree_, focus_.get());
    auto input = EventResponse{};
    for (auto const& [index, node] : path) {
        if (result.quit) { break; }
        if (accepts_input(tree_, index, node.widget)) {
            input = combine(input, callback(node.widget.get()));
            if (!input.quit) { result = combine(result, synchronize()); }
            if (input.handled || input.quit) { break; }
        }
    }
    result = combine(result, input);
    report(result);
    return result;
}

auto Application::handle_mouse_press(MousePress m) -> EventResponse
{ return route_mouse(m, &Widget::mouse_press, m.button == MouseButton::Left); }
auto Application::handle_mouse_release(MouseRelease m) -> EventResponse
{ return route_mouse(m, &Widget::mouse_release); }
auto Application::handle_mouse_wheel(MouseWheel m) -> EventResponse
{ return route_mouse(m, &Widget::mouse_wheel); }
auto Application::handle_mouse_move(MouseMove m) -> EventResponse
{ return route_mouse(m, &Widget::mouse_move); }

auto Application::handle_key_press(KeyEvent key) -> EventResponse
{
    if (key.matches(KeyCode::Tab, {}, key.action) ||
        key.matches(KeyCode::Tab, {.shift = true}, key.action)) {
        auto result = synchronize();
        if (!result.quit) {
            result = combine(result, focus_.advance(tree_, key.modifiers.shift));
        }
        result.handled = true;
        report(result);
        return result;
    }
    return route_focused([&](Widget& widget) { return widget.key_press(key); });
}
auto Application::handle_key_release(KeyEvent key) -> EventResponse
{
    return route_focused([&](Widget& widget) { return widget.key_release(key); });
}
auto Application::handle_paste(esc::PasteChunk const& chunk) -> EventResponse
{
    auto result = synchronize();
    if (result.quit) { return result; }
    if (chunk.first) {
        paste_target_ = {};
        auto const path = ancestry(tree_, focus_.get());
        for (auto const& [index, node] : path) {
            auto const target = node.widget;
            if (accepts_input(tree_, index, target)) {
                auto const response = target.get().paste(chunk);
                result = combine(result, response);
                if (response.handled && !chunk.last && !chunk.aborted) {
                    paste_target_ = target;
                }
                if (response.quit) { break; }
                result = combine(result, synchronize());
                if (response.handled || result.quit) { break; }
            }
        }
    }
    else {
        auto target = paste_target_;
        if (chunk.last || chunk.aborted) { paste_target_ = {}; }
        if (accepts_input(tree_, tree_.size(), target)) {
            auto const response = target.get().paste(chunk);
            result = combine(result, response);
            if (!response.quit) { result = combine(result, synchronize()); }
        }
        else {
            paste_target_ = {};
        }
    }
    report(result);
    return result;
}

auto Application::handle_resize(Area new_size) -> EventResponse
{
    validate(new_size);
    auto const old_size = head_.size;
    head_.size = new_size;
    head_.resize(old_size);
    return combine({.handled = true, .repaint = true}, synchronize());
}

auto Application::handle_paint(Canvas canvas) -> Cursor
{
    synchronize();
    auto const snapshot = tree_;
    auto cursor = Cursor{};
    if (!snapshot.empty()) { paint_tree(snapshot, 0, canvas, focus_.get(), cursor); }
    return cursor;
}
}  // namespace ox
