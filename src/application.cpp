#include <ox/application.hpp>
#include <ox/put.hpp>

#include <algorithm>
#include <cstdint>
#include <limits>
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
    node.widget.get().paint_background(canvas);
    if (!node.widget.valid()) { return; }
    auto const local = node.widget.get().paint(canvas);
    if (node.widget.valid() && node.widget == focus) {
        cursor = local && canvas.contains(*local) ? Cursor{canvas.to_global(*local)}
                                                  : Cursor{};
    }
    for (auto child = index + 1; child < node.end; child = tree[child].end) {
        if (!node.widget.valid()) { return; }
        if (index == 0 && tree[child].widget.valid() &&
            tree[child].widget.get().overlay) {
            continue;
        }
        auto const& bounds = tree[child].bounds;
        auto const offset =
            Point{bounds.at.x - node.bounds.at.x, bounds.at.y - node.bounds.at.y};
        paint_tree(tree, child, canvas.subcanvas(offset, bounds.size), focus, cursor);
    }
    if (node.widget.valid()) { node.widget.get().paint_overlay(canvas); }
}
}  // namespace

Application::Application(Widget& head) : head_{track(head)}, focus_{head}
{
    clipboard.read = [this] { return clipboard_text_; };
    clipboard.write = [this](std::string_view text) { clipboard_text_ = text; };
    try {
        tree_ = widget_tree(head_.get());
        attach_widgets();
        attachments_current_ = true;
        loop_.apply(focus_.reconcile(tree_));
    }
    catch (...) {
        for (auto const& target : attached_) {
            if (target.valid() && target.get().application_ == this) {
                for (auto const& timer : target.get().timers_) {
                    loop_.unwatch(timer);
                }
                for (auto const& a : target.get().animations_)
                    loop_.unwatch(a);
                target.get().focused_ = false;
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
            for (auto const& a : target.get().animations_)
                loop_.unwatch(a);
            target.get().focused_ = false;
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
        widget.input_enabled_ = node.input;
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
        for (auto const& a : widget.animations_)
            loop_.unwatch(a);
    }
    attached_.swap(current);
    attachment_storage_ = std::move(current);
    for (auto const& target : attached_) {
        auto& widget = target.get();
        if (widget.application_ == this) { continue; }
        for (auto const& timer : widget.timers_) {
            loop_.watch(timer);
        }
        for (auto const& a : widget.animations_)
            loop_.watch(a);
        widget.application_ = this;
        if (theme_.styled) layout_pending_ = true;
        if (widget.overlay) {
            if (widget.visible) { widget.overlay_order = ++overlay_order_; }
            layout_pending_ = true;
        }
        std::erase_if(widget.timers_,
                      [](auto const& timer) { return timer.expired(); });
        widget.theme_changed();
    }
}

void Application::raise_overlay(Widget& widget)
{
    if (widget.application_ != this || !widget.overlay) {
        throw std::invalid_argument{"Not an attached overlay"};
    }
    widget.overlay_order = ++overlay_order_;
    request_repaint();
}

auto Application::bounds(Widget const& widget) const -> Rect
{
    if (!head_.valid()) { throw std::logic_error{"Application root unavailable"}; }
    auto const tree = widget_tree(head_.get());
    auto const target = track(const_cast<Widget&>(widget));
    auto const found = std::ranges::find(tree, target, &WidgetNode::widget);
    if (found == tree.end()) {
        throw std::invalid_argument{"Widget outside application"};
    }
    return found->bounds;
}

auto Application::is_interactive(Widget const& widget) const -> bool
{
    if (!head_.valid()) { return false; }
    auto const tree = widget_tree(head_.get());
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
    if (head_.valid()) { widget_tree(head_.get(), next); }
    else {
        next.clear();
    }
    auto const changed = next != tree_;
    tree_.swap(next);
    tree_storage_ = std::move(next);
    shortcuts_.clear();
    for (std::size_t i = 0; i < tree_.size(); ++i) {
        if (auto* widget = tree_[i].widget.try_get(); widget && widget->shortcuts) {
            shortcuts_.push_back({i, tree_[i].widget});
        }
    }
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
    auto result = EventResponse{};
    for (;;) {
        if (!head_.valid()) {
            loop_.quit(0);
            return {.quit = 0};
        }
        while (std::exchange(layout_pending_, false)) {
            arrange(head_.get(), head_.get().bounds());
            if (!head_.valid()) { break; }
            auto overlays = widget_tree(head_.get());
            auto extent = head_.get().size();
            for (auto const& node : overlays) {
                if (auto* widget = node.widget.try_get(); widget && widget->overlay) {
                    arrange(*widget, {{}, extent});
                }
                if (!head_.valid()) { break; }
            }
        }
        if (!head_.valid()) {
            loop_.quit(0);
            return {.quit = 0};
        }
        result = combine(result, {.repaint = refresh_tree()});
        if (layout_pending_) { continue; }
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
        auto captured = pointer_capture_;
        if (captured.valid() && !accepts_input(tree_, tree_.size(), captured)) {
            pointer_capture_ = {};
            ++capture_revision_;
            result = combine(result, captured.get().pointer_cancel());
        }
        if (!pointer_capture_.valid()) { pointer_capture_ = {}; }
        if (result.quit || (!layout_pending_ && head_.valid())) { return result; }
    }
}

auto Application::run(Terminal::Options options) -> int
{
    if (auto code = loop_.exit_code()) { return *code; }
    auto terminal = Terminal{options};
    return process_events(loop_, terminal, *this, [this] { return synchronize(); });
}

auto Application::step(EventLoop::Clock::time_point now) -> EventResponse
{
    auto result = synchronize();
    loop_.apply(result);
    if (!result.quit) { result = combine(result, loop_.step(*this, now)); }
    if (!result.quit) {
        auto const response = synchronize();
        loop_.apply(response);
        result = combine(result, response);
    }
    return result;
}

auto Application::paint() -> bool
{
    loop_.apply(synchronize());
    return loop_.paint(*this);
}
void Application::post(Event event) { loop_.post(std::move(event)); }
void Application::set_theme(Theme theme)
{
    validate(theme);
    theme_ = std::move(theme);
    loop_.next_frame_.reset();
    request_layout();
    auto snapshot = attached_;
    for (auto const& target : snapshot)
        if (auto* widget = target.try_get(); widget && widget->application_ == this)
            widget->theme_changed();
}
void Application::request_repaint() { loop_.request_repaint(); }
void Application::request_layout()
{
    layout_pending_ = true;
    loop_.request_repaint();
}
void Application::quit(int code) { loop_.quit(code); }

void Application::set_focus(Widget& widget) { loop_.apply(focus_.set(widget)); }
void Application::clear_focus() { loop_.apply(focus_.clear()); }
auto Application::dispatch(Event const& event) -> EventResponse
{
    auto result = synchronize();
    if (!result.quit) {
        result = combine(result, ox::dispatch(event, *this, loop_.screen));
    }
    loop_.apply(result);
    result.quit = loop_.exit_code();
    return result;
}

auto Application::capture_pointer(Widget& widget, MouseButton button) -> bool
{
    if (!is_interactive(widget)) { return false; }
    auto previous = pointer_capture_;
    auto target = track(widget);
    pointer_capture_ = target;
    capture_button_ = button;
    ++capture_revision_;
    if (previous.valid() && previous != target) {
        loop_.apply(previous.get().pointer_cancel());
    }
    return pointer_capture_ == target && target.valid();
}
void Application::release_pointer(Widget& widget)
{
    if (pointer_capture_ == track(widget)) {
        pointer_capture_ = {};
        ++capture_revision_;
    }
}

template <typename Input>
auto Application::route_mouse(Input event, bool select) -> EventResponse
{
    mouse_position_ = event.at;
    auto result = synchronize();
    auto const captured = pointer_capture_;
    auto const revision = capture_revision_;
    auto use_capture = false;
    if constexpr (std::same_as<Input, MouseMove>) { use_capture = captured.valid(); }
    if constexpr (std::same_as<Input, MouseRelease>) {
        use_capture =
            captured.valid() && (!event.button || event.button == capture_button_);
    }
    auto const path = use_capture ? ancestry(tree_, captured)
                                  : ancestry(tree_, hit_test(tree_, event.at));
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
        auto difference = [](int position, int origin) {
            return (int)std::clamp((std::int64_t)position - origin,
                                   (std::int64_t)std::numeric_limits<int>::min(),
                                   (std::int64_t)std::numeric_limits<int>::max());
        };
        local.at = {difference(event.at.x, node.bounds.at.x),
                    difference(event.at.y, node.bounds.at.y)};
        input = combine(input, node.widget.get().handle(local));
        if (!input.quit) { result = combine(result, synchronize()); }
        if (input.handled || input.quit) { break; }
    }
    if constexpr (std::same_as<Input, MouseRelease>) {
        if (use_capture && revision == capture_revision_) {
            pointer_capture_ = {};
            ++capture_revision_;
        }
    }
    result = combine(result, input);
    return result;
}

template <typename Input>
auto Application::route_focused(Input const& event) -> EventResponse
{
    auto result = synchronize();
    auto const path = ancestry(tree_, focus_.get());
    auto input = EventResponse{};
    for (auto const& [index, node] : path) {
        if (result.quit) { break; }
        if (accepts_input(tree_, index, node.widget)) {
            input = combine(input, node.widget.get().handle(event));
            if (!input.quit) { result = combine(result, synchronize()); }
            if (input.handled || input.quit) { break; }
        }
    }
    result = combine(result, input);
    return result;
}

auto Application::handle(MousePress const& event) -> EventResponse
{ return route_mouse(event, event.button == MouseButton::Left); }
auto Application::handle(MouseRelease const& event) -> EventResponse
{ return route_mouse(event); }
auto Application::handle(MouseWheel const& event) -> EventResponse
{ return route_mouse(event); }
auto Application::handle(MouseMove const& event) -> EventResponse
{ return route_mouse(event); }

auto Application::handle(KeyEvent const& key) -> EventResponse
{
    auto result = synchronize();
    if (result.quit) { return result; }
    auto snapshot = shortcuts_;
    for (auto const& [index, target] : snapshot) {
        if (!accepts_input(tree_, index, target)) { continue; }
        auto callback = target.get().shortcuts;
        if (!callback) { continue; }
        auto effect = callback(key);
        result = combine(result, effect);
        if (!effect.quit) { result = combine(result, synchronize()); }
        if (effect.handled || result.quit) { return result; }
    }
    if (key.action != KeyAction::Release &&
        (key.matches(KeyCode::Tab, {}, key.action) ||
         key.matches(KeyCode::Tab, {.shift = true}, key.action))) {
        result = combine(result, synchronize());
        if (!result.quit) {
            result = combine(result, focus_.advance(tree_, key.modifiers.shift));
        }
        result.handled = true;
        return result;
    }
    return combine(result, route_focused(key));
}
auto Application::handle(esc::PasteChunk const& chunk) -> EventResponse
{
    auto result = synchronize();
    if (result.quit) { return result; }
    if (chunk.first) {
        paste_target_ = {};
        auto const path = ancestry(tree_, focus_.get());
        for (auto const& [index, node] : path) {
            auto const target = node.widget;
            if (accepts_input(tree_, index, target)) {
                auto const response = target.get().handle(chunk);
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
            auto const response = target.get().handle(chunk);
            result = combine(result, response);
            if (!response.quit) { result = combine(result, synchronize()); }
        }
        else {
            paste_target_ = {};
        }
    }
    return result;
}

auto Application::handle(esc::Resize const& event) -> EventResponse
{
    if (!head_.valid()) { return {.quit = 0}; }
    arrange(head_.get(), {{0, 0}, event.size});
    layout_pending_ = true;
    return combine({.handled = true, .repaint = true}, synchronize());
}

auto EventLoop::paint(Application& app) -> bool
{
    if (!repaint_ || exit_code_) return false;
    repaint_ = false;
    screen.clear(app.theme_.styled ? theme_style({}, app.theme_) : Style{});
    cursor = app.paint_widgets(Canvas{screen});
    return true;
}

auto Application::paint(Canvas canvas) -> Cursor
{
    if (theme_.styled) fill(canvas, theme_style({}, theme_));
    return paint_widgets(canvas);
}

auto Application::paint_widgets(Canvas canvas) -> Cursor
{
    auto const snapshot = tree_;
    auto cursor = Cursor{};
    if (!snapshot.empty()) {
        paint_tree(snapshot, 0, canvas, focus_.get(), cursor);
        for (auto child = std::size_t{1}; child < snapshot[0].end;
             child = snapshot[child].end) {
            if (!snapshot[0].widget.valid()) { break; }
            if (snapshot[child].widget.valid() &&
                snapshot[child].widget.get().overlay) {
                auto bounds = snapshot[child].bounds;
                paint_tree(snapshot, child, canvas.subcanvas(bounds.at, bounds.size),
                           focus_.get(), cursor);
            }
        }
    }
    return cursor;
}
}  // namespace ox
