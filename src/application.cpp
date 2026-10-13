#include <ox/application.hpp>

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace ox {
namespace {
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
        auto const order = focus_order(tree_);
        if (!order.empty()) { focus_.set(tree_[order.front()].widget.get()); }
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
    auto current = std::vector<LifetimeView<Widget>>{};
    current.reserve(tree_.size());
    for (auto const& node : tree_) {
        if (!node.widget.valid()) { continue; }
        auto& widget = node.widget.get();
        if (widget.application_ && widget.application_ != this) {
            throw std::logic_error{"Widget already belongs to an application"};
        }
        current.push_back(node.widget);
    }
    if (current == attached_) { return; }
    for (auto const& target : attached_) {
        if (!target.valid() || std::ranges::find(current, target) != current.end()) {
            continue;
        }
        auto& widget = target.get();
        widget.application_ = nullptr;
        for (auto const& entry : widget.timers_) {
            loop_.unwatch(entry);
        }
    }
    attached_ = std::move(current);
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

auto Application::update_hover() -> EventResponse
{
    auto next = std::vector<LifetimeView<Widget>>{};
    if (mouse_position_) {
        for (auto index : hit_path(tree_, *mouse_position_)) {
            next.push_back(tree_[index].widget);
        }
    }
    if (next == hovered_) { return {}; }
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
            if (result.quit) { return result; }
        }
    }
    for (auto i = common; i < entering.size(); ++i) {
        if (entering[i].valid() && is_interactive(entering[i].get())) {
            result = combine(result, entering[i].get().mouse_enter());
            if (result.quit) { break; }
        }
    }
    return result;
}

auto Application::synchronize() -> EventResponse
{
    if (auto code = loop_.exit_code()) { return {.quit = code}; }
    auto next = widget_tree(head_);
    auto result = EventResponse{.repaint = next != tree_};
    tree_ = std::move(next);
    attach_widgets();
    // Keep the snapshot alive across callbacks that can reenter the application.
    auto snapshot = tree_;
    auto const focus_response = focus_.reconcile(snapshot);
    result = combine(result, focus_response);
    if (focus_response.repaint || focus_response.handled || focus_response.quit) {
        tree_ = widget_tree(head_);
        attach_widgets();
    }
    if (!result.quit) {
        auto const previous_hover = hovered_;
        result = combine(result, update_hover());
        if (!result.quit && previous_hover != hovered_) {
            auto current = widget_tree(head_);
            result.repaint |= current != tree_;
            tree_ = std::move(current);
            attach_widgets();
            auto stable = tree_;
            auto const response = focus_.reconcile(stable);
            result = combine(result, response);
            if (response.repaint || response.handled || response.quit) {
                tree_ = widget_tree(head_);
                attach_widgets();
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
    auto snapshot = tree_;
    auto const path = hit_path(snapshot, event.at);
    if (select && !result.quit) {
        for (auto it = path.rbegin(); it != path.rend(); ++it) {
            auto const target = snapshot[*it].widget;
            if (!target.valid() || !is_interactive(target.get())) { continue; }
            auto const policy = target.get().focus_policy;
            if (policy == FocusPolicy::Click || policy == FocusPolicy::Strong) {
                result = combine(result, focus_.set(target.get()));
                break;
            }
        }
    }
    if (select && !result.quit) { result = combine(result, synchronize()); }
    // Focus/hover effects do not consume the input event itself.
    auto input = EventResponse{};
    for (auto it = path.rbegin(); it != path.rend() && !result.quit; ++it) {
        auto const& node = snapshot[*it];
        if (!node.widget.valid() ||
            !std::ranges::any_of(tree_, [&](auto const& current) {
                return current.input && current.widget == node.widget;
            })) {
            continue;
        }
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
    auto const focused = focus_.get();
    auto snapshot = tree_;
    auto target = std::optional<std::size_t>{};
    for (auto i = std::size_t{0}; i < snapshot.size(); ++i) {
        if (snapshot[i].widget == focused) {
            target = i;
            break;
        }
    }
    auto input = EventResponse{};
    while (target && !result.quit) {
        auto const& node = snapshot[*target];
        if (node.widget.valid() && std::ranges::any_of(tree_, [&](auto const& current) {
                return current.input && current.widget == node.widget;
            })) {
            input = combine(input, callback(node.widget.get()));
            if (!input.quit) { result = combine(result, synchronize()); }
            if (input.handled || input.quit) { break; }
        }
        target = node.parent;
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
        auto snapshot = tree_;
        if (!result.quit) {
            result = combine(result, focus_.advance(snapshot, key.modifiers.shift));
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
        auto const focused = focus_.get();
        auto const snapshot = tree_;
        auto index = std::optional<std::size_t>{};
        for (auto i = std::size_t{0}; i < snapshot.size(); ++i) {
            if (snapshot[i].widget == focused) {
                index = i;
                break;
            }
        }
        while (index) {
            auto const target = snapshot[*index].widget;
            if (target.valid() && is_interactive(target.get())) {
                auto const response = target.get().paste(chunk);
                result = combine(result, response);
                if (response.handled && !chunk.last && !chunk.aborted) {
                    paste_target_ = target;
                }
                if (response.quit) { break; }
                result = combine(result, synchronize());
                if (response.handled || result.quit) { break; }
            }
            index = snapshot[*index].parent;
        }
    }
    else {
        auto target = paste_target_;
        if (chunk.last || chunk.aborted) { paste_target_ = {}; }
        if (target.valid() && is_interactive(target.get())) {
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
