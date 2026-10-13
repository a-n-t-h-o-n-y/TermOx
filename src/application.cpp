#include <ox/application.hpp>

#include <algorithm>
#include <chrono>
#include <ranges>
#include <utility>

#include <stdexcept>

#include <ox/core/core.hpp>
#include <ox/focus.hpp>

namespace {

using namespace ox;

/**
 * Find the Widget in \p widgets that contains the Point \p p.
 *
 * @details This does not check descendants of the Widgets in \p widgets. \p p is
 * relative to the parent of the passed in Widgets; the top left corner of the parent is
 * {0, 0}.
 * @param widgets The Widgets to search.
 * @param p The Point to search for.
 * @returns A pointer to the Widget that contains \p p, or nullptr if no Widget.
 */
template <InputRangeOf<Widget> R>
[[nodiscard]] auto find_widget_at(R&& widgets, Point p) -> Widget*
{
    auto const at = std::ranges::find_if(widgets, [p](Widget& child) {
        return child.at.x <= p.x && p.x < child.at.x + child.size.width &&
               child.at.y <= p.y && p.y < child.at.y + child.size.height;
    });

    return (at == std::end(widgets)) ? nullptr : &*at;
}

/**
 * Execute \p fn on \p w and every descendant of \p w, in a depth first traversal.
 *
 * @param w The Widget to start the traversal from.
 * @param fn The function to execute on each Widget.
 */
void for_each_depth_first(Widget& w, std::function<void(Widget&)> const& fn)
{
    fn(w);
    for (Widget& child : w.get_children()) {
        for_each_depth_first(child, fn);
    }
}

/**
 * Find the first Widget in \p w and its descendants that satisfies \p predicate.
 *
 * @details This includes \p w itself in the search.
 * @param w The Widget to start the search from.
 * @param predicate The function to test each Widget with.
 * @returns A pointer to the first Widget that satisfies \p predicate, or nullptr if no
 * Widget is found.
 */
[[nodiscard]] auto find_if_depth_first(
    Widget& w,
    std::function<bool(Widget const&)> const& predicate) -> Widget*
{
    Widget* result = nullptr;
    for_each_depth_first(w, [&](Widget& child) {
        if (result == nullptr && predicate(child)) { result = &child; }
    });
    return result;
}

// -------------------------------------------------------------------------------------

enum class SetFocus : bool { Yes, No };

template <SetFocus SF, typename MouseEvent, typename EventFn>
void any_mouse_event(Focus& focus, Widget& head, MouseEvent m, EventFn&& event_fn)
{
    Widget* const found_ptr =
        find_widget_at(head.get_children() | filter::is_active, m.at);

    if (found_ptr == nullptr) {  // `head` is last Widget that contains m.at
        if constexpr (SF == SetFocus::Yes) {
            if (head.focus_policy == FocusPolicy::Strong ||
                head.focus_policy == FocusPolicy::Click) {
                focus.set(head);
            }
        }
        std::forward<EventFn>(event_fn)(head, m);
    }
    else {
        m.at = m.at - found_ptr->at;
        any_mouse_event<SF>(focus, *found_ptr, m, event_fn);
    }
}

void send_leave_events(Widget& w, Point p)
{
    w.mouse_leave();
    Widget* const next = find_widget_at(w.get_children() | filter::is_active, p);
    if (next != nullptr) { send_leave_events(*next, p - next->at); }
}

void send_enter_events(Widget& w, Point p)
{
    w.mouse_enter();
    Widget* const next = find_widget_at(w.get_children() | filter::is_active, p);
    if (next != nullptr) { send_enter_events(*next, p - next->at); }
}

void send_enter_leave_events(Widget& w, Point previous, Point current)
{
    Widget* const previous_widget =
        find_widget_at(w.get_children() | filter::is_active, previous);
    Widget* const current_widget =
        find_widget_at(w.get_children() | filter::is_active, current);

    if (previous_widget != nullptr && current_widget != nullptr &&
        previous != current) {
        send_leave_events(*previous_widget, previous - previous_widget->at);
        send_enter_events(*current_widget, current - current_widget->at);
    }
}

// -------------------------------------------------------------------------------------

void do_tab_focus_change(Focus& focus, Widget& head, Widget const& current_focus)
{
    Widget* next = find_if_depth_first(
        head, [found = false, &current_focus](Widget const& w) mutable {
            if (found) {
                return w.focus_policy == FocusPolicy::Strong ||
                       w.focus_policy == FocusPolicy::Tab;
            }
            else {
                if (&w == &current_focus) { found = true; }
                return false;
            }
        });

    if (next == nullptr) {
        next = find_if_depth_first(head, [](Widget const& w) {
            return w.focus_policy == FocusPolicy::Strong ||
                   w.focus_policy == FocusPolicy::Tab;
        });
    }

    if (next != nullptr && next != &current_focus) { focus.set(*next); }
}

void do_shift_tab_focus_change(Focus& focus, Widget& head, Widget const& current_focus)
{
    Widget* next = nullptr;

    for_each_depth_first(
        head, [&next, previous = (Widget*)nullptr, &current_focus](Widget& w) mutable {
            if (&w == &current_focus) { next = previous; }
            if (w.focus_policy == FocusPolicy::Strong ||
                w.focus_policy == FocusPolicy::Tab) {
                previous = &w;
            }
        });

    if (next == nullptr) {
        for_each_depth_first(head, [&next](Widget& w) {
            if (w.focus_policy == FocusPolicy::Strong ||
                w.focus_policy == FocusPolicy::Tab) {
                next = &w;
            }
        });
    }

    if (next != nullptr && next != &current_focus) { focus.set(*next); }
}

// Recursively send paint events to each Widget including and below head. \p cursor is
// assigned to if the current focus widget is painted.
void send_paint_events(Focus const& focus,
                       Widget& head,
                       Canvas canvas,
                       Cursor& cursor_out)
{
    for (auto& child : head.get_children() | filter::is_active) {
        send_paint_events(focus, child, canvas.subcanvas(child.at, child.size),
                          cursor_out);
    }
    if (head.active && head.size.width > 0 && head.size.height > 0) {
        head.paint(canvas);
        if (auto const life = focus.get(); life.valid() && &(life.get()) == &head) {
            cursor_out = head.cursor && canvas.contains(*head.cursor)
                             ? Cursor{canvas.to_global(*head.cursor)}
                             : Cursor{};
        }
    }
}

}  // namespace

namespace ox {

Application::Application(Widget& head) : head_{head}
{
    try {
        this->attach_widgets();
        if (auto initial = find_if_depth_first(head_, [](Widget const& widget) {
                return widget.active && (widget.focus_policy == FocusPolicy::Strong ||
                                         widget.focus_policy == FocusPolicy::Tab);
            })) {
            focus_.set(*initial);
        }
    }
    catch (...) {
        for (auto const& target : attached_) {
            if (target.valid() && target.get().application_ == this) {
                for (auto const& timer : target.get().timers_) {
                    loop_.unwatch(timer);
                }
                target.get().application_ = nullptr;
            }
        }
        throw;
    }
}

Application::~Application()
{
    focus_.clear();
    for (auto const& target : attached_) {
        if (target.valid() && target.get().application_ == this) {
            for (auto const& timer : target.get().timers_) {
                loop_.unwatch(timer);
            }
            target.get().application_ = nullptr;
        }
    }
}

void Application::attach_widgets()
{
    auto current = std::vector<LifetimeView<Widget>>{};
    for_each_depth_first(head_, [&](Widget& widget) {
        if (widget.application_ && widget.application_ != this) {
            throw std::logic_error{"Widget already belongs to an application"};
        }
        current.push_back(track(widget));
    });
    for (auto const& target : attached_) {
        if (!target.valid()) { continue; }
        auto& widget = target.get();
        if (std::ranges::none_of(
                current, [&](auto const& entry) { return &entry.get() == &widget; })) {
            if (widget.focused) { focus_.clear(); }
            widget.application_ = nullptr;
            for (auto const& entry : widget.timers_) {
                loop_.unwatch(entry);
            }
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

auto Application::run(Terminal::Options options) -> int
{
    if (auto code = loop_.exit_code()) { return *code; }
    auto terminal = Terminal{options};
    return process_events(loop_, terminal, *this, [this] { this->attach_widgets(); });
}

auto Application::step(EventLoop::Clock::time_point now) -> EventResponse
{
    this->attach_widgets();
    auto result = loop_.step(*this, now);
    this->attach_widgets();
    return result;
}

auto Application::paint() -> bool { return loop_.paint(*this); }
void Application::post(Event event) { loop_.post(std::move(event)); }
void Application::request_repaint() { loop_.request_repaint(); }
void Application::quit(int code) { loop_.quit(code); }

auto Application::handle_mouse_press(MousePress m) -> EventResponse
{
    ::any_mouse_event<SetFocus::Yes>(focus_, head_, m,
                                     [](Widget& w, MousePress m) { w.mouse_press(m); });
    return {.handled = true, .repaint = true};
}

auto Application::handle_mouse_release(MouseRelease m) -> EventResponse
{
    ::any_mouse_event<SetFocus::No>(
        focus_, head_, m, [](Widget& w, MouseRelease m) { w.mouse_release(m); });
    return {.handled = true, .repaint = true};
}

auto Application::handle_mouse_wheel(MouseWheel m) -> EventResponse
{
    ::any_mouse_event<SetFocus::No>(focus_, head_, m,
                                    [](Widget& w, MouseWheel m) { w.mouse_wheel(m); });
    return {.handled = true, .repaint = true};
}

auto Application::handle_mouse_move(MouseMove m) -> EventResponse
{
    ::send_enter_leave_events(head_, previous_mouse_position_, m.at);
    ::any_mouse_event<SetFocus::No>(focus_, head_, m,
                                    [](Widget& w, MouseMove m) { w.mouse_move(m); });
    previous_mouse_position_ = m.at;
    return {.handled = true, .repaint = true};
}

auto Application::handle_key_press(KeyEvent k) -> EventResponse
{
    auto const life = focus_.get();

    if (not life.valid()) { return {}; }
    auto& focused = life.get();
    if (focused.focus_policy == FocusPolicy::Strong ||
        focused.focus_policy == FocusPolicy::Tab) {
        if (k.matches(KeyCode::Tab)) { do_tab_focus_change(focus_, head_, focused); }
        else if (k.matches(KeyCode::Tab, {.shift = true})) {
            do_shift_tab_focus_change(focus_, head_, focused);
        }
    }

    focused.key_press(k);

    return {.handled = true, .repaint = true};
}

auto Application::handle_key_release(KeyEvent k) -> EventResponse
{
    auto const life = focus_.get();
    if (!life.valid()) { return {}; }
    life.get().key_release(k);
    return {.handled = true, .repaint = true};
}

auto Application::handle_paste(esc::PasteChunk const& chunk) -> EventResponse
{
    auto const life = focus_.get();
    if (!life.valid()) { return {}; }
    life.get().paste(chunk);
    return {.handled = true, .repaint = true};
}

auto Application::handle_resize(Area new_size) -> EventResponse
{
    auto const old_size = head_.size;
    head_.size = new_size;
    head_.resize(old_size);
    return {.handled = true, .repaint = true};
}

auto Application::handle_paint(Canvas canvas) -> Cursor
{
    this->attach_widgets();
    auto cursor = Cursor{std::nullopt};
    ::send_paint_events(focus_, head_, canvas, cursor);
    return cursor;
}

}  // namespace ox