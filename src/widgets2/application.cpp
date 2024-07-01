#include <ox/widgets2/application.hpp>

#include <algorithm>
#include <ranges>

#include <ox/core.hpp>
#include <ox/widgets2/focus.hpp>

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
template <std::ranges::input_range R>
[[nodiscard]] auto find_widget_at(R&& widgets, Point p) -> Widget*
{
    auto const at = std::ranges::find_if(widgets, [p](Widget& child) {
        return child.enabled && child.at.x <= p.x &&
               p.x < child.at.x + child.size.width && child.at.y <= p.y &&
               p.y < child.at.y + child.size.height;
    });

    return (at == std::end(widgets)) ? nullptr : &*at;
}

/**
 * Execute \p fn on \p w and every descendant of \p w, in a depth first traversal.
 *
 * @param w The Widget to start the traversal from.
 * @param fn The function to execute on each Widget.
 */
auto for_each_depth_first(Widget& w, std::function<void(Widget&)> const& fn) -> void
{
    fn(w);
    for (Widget& child : w.get_children()) {
        for_each_depth_first(child, fn);
    }
}

/**
 * Execute \p fn on \p w and every descendant of \p w, in a depth first traversal.
 *
 * @param w The Widget to start the traversal from.
 * @param fn The function to execute on each Widget.
 */
// auto for_each_depth_first(Widget const& w,
//                           std::function<void(Widget const&)> const& fn) -> void
// {
//     fn(w);
//     for (Widget const& child : w.get_children()) {
//         for_each_depth_first(child, fn);
//     }
// }

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
        if (result == nullptr && predicate(child)) {
            result = &child;
        }
    });
    return result;
}

// -------------------------------------------------------------------------------------

enum class SetFocus : bool { Yes, No };

template <SetFocus SF, typename EventFn>
auto any_mouse_event(Widget& head, Mouse m, EventFn&& event_fn) -> void
{
    Widget* const found_ptr = find_widget_at(head.get_children(), m.at);

    if (found_ptr == nullptr) {  // `head` is last Widget that contains m.at
        if constexpr (SF == SetFocus::Yes) {
            if (head.focus_policy == FocusPolicy::Strong ||
                head.focus_policy == FocusPolicy::Click) {
                Focus::set(head);
            }
        }
        std::forward<EventFn>(event_fn)(head, m);
    }
    else {
        m.at = m.at - found_ptr->at;
        any_mouse_event<SF>(*found_ptr, m, event_fn);
    }
}

auto send_leave_events(Widget& w, Point p) -> void
{
    w.mouse_leave();
    Widget* const next = find_widget_at(w.get_children(), p);
    if (next != nullptr) {
        send_leave_events(*next, p - next->at);
    }
}

auto send_enter_events(Widget& w, Point p) -> void
{
    w.mouse_enter();
    Widget* const next = find_widget_at(w.get_children(), p);
    if (next != nullptr) {
        send_enter_events(*next, p - next->at);
    }
}

auto send_enter_leave_events(Widget& w, Point previous, Point current) -> void
{
    Widget* const previous_widget = find_widget_at(w.get_children(), previous);
    Widget* const current_widget = find_widget_at(w.get_children(), current);

    if (previous_widget != nullptr && current_widget != nullptr &&
        previous != current) {
        send_leave_events(*previous_widget, previous - previous_widget->at);
        send_enter_events(*current_widget, current - current_widget->at);
    }
}

// -------------------------------------------------------------------------------------

auto do_tab_focus_change(Widget& head, Widget const* current_focus) -> void
{
    Widget* next = find_if_depth_first(
        head, [found = false, current_focus](Widget const& w) mutable {
            if (found) {
                return w.focus_policy == FocusPolicy::Strong ||
                       w.focus_policy == FocusPolicy::Tab;
            }
            else {
                if (&w == current_focus) {
                    found = true;
                }
                return false;
            }
        });

    if (next == nullptr) {
        next = find_if_depth_first(head, [](Widget const& w) {
            return w.focus_policy == FocusPolicy::Strong ||
                   w.focus_policy == FocusPolicy::Tab;
        });
    }

    if (next != nullptr && next != current_focus) {
        Focus::set(*next);
    }
}

auto do_shift_tab_focus_change(Widget& head, Widget const* current_focus) -> void
{
    Widget* next = nullptr;

    for_each_depth_first(
        head, [&next, previous = (Widget*)nullptr, current_focus](Widget& w) mutable {
            if (&w == current_focus) {
                next = previous;
            }
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

    if (next != nullptr && next != current_focus) {
        Focus::set(*next);
    }
}

// Recursively send paint events to each Widget including and below head. \p cursor is
// assigned to if the current focus widget is painted.
auto send_paint_events(Widget const& head,
                       Canvas canvas,
                       Terminal::Cursor& cursor_out) -> void
{
    if (head.enabled && head.size.width > 0 && head.size.height > 0) {
        head.paint(canvas);
        if (Focus::get() == &head) {
            cursor_out = head.cursor ? canvas.at + *head.cursor : head.cursor;
        }
    }
    for (auto const& child : head.get_children()) {
        send_paint_events(child,
                          Canvas{
                              .buffer = canvas.buffer,
                              .at = canvas.at + child.at,
                              .size = child.size,
                          },
                          cursor_out);
    }
}

}  // namespace

namespace ox {

Application::Application(Widget& head, Terminal term)
    : head_{head}, term_{std::move(term)}
{}

auto Application::run() -> int { return process_events(term_, *this); }

auto Application::handle_mouse_press(Mouse m) -> EventResponse
{
    ::any_mouse_event<SetFocus::Yes>(head_, m,
                                     [](Widget& w, Mouse m) { w.mouse_press(m); });
    return {};
}

auto Application::handle_mouse_release(Mouse m) -> EventResponse
{
    ::any_mouse_event<SetFocus::No>(head_, m,
                                    [](Widget& w, Mouse m) { w.mouse_release(m); });
    return {};
}

auto Application::handle_mouse_wheel(Mouse m) -> EventResponse
{
    ::any_mouse_event<SetFocus::No>(head_, m,
                                    [](Widget& w, Mouse m) { w.mouse_wheel(m); });
    return {};
}

auto Application::handle_mouse_move(Mouse m) -> EventResponse
{
    ::send_enter_leave_events(head_, previous_mouse_position_, m.at);
    ::any_mouse_event<SetFocus::No>(head_, m,
                                    [](Widget& w, Mouse m) { w.mouse_move(m); });
    previous_mouse_position_ = m.at;
    return {};
}

auto Application::handle_key_press(Key k) -> EventResponse
{
    // TODO a shortcuts manager? processed here?
    Widget* const focused = Focus::get();
    if (focused == nullptr) {
        return {};
    }

    if (focused->focus_policy == FocusPolicy::Strong ||
        focused->focus_policy == FocusPolicy::Tab) {
        if (k == Key::Tab) {
            do_tab_focus_change(head_, focused);
        }
        else if (k == Key::BackTab) {
            do_shift_tab_focus_change(head_, focused);
        }
    }

    focused->key_press(k);

    return {};
}

auto Application::handle_key_release(Key k) -> EventResponse
{
    Widget* const focused = Focus::get();
    if (focused != nullptr) {
        focused->key_release(k);
    }
    return {};
}

auto Application::handle_resize(Area new_size) -> EventResponse
{
    head_.size = new_size;
    head_.resize(new_size);
    return {};
}

auto Application::handle_timer(int id) -> EventResponse
{
    for_each_depth_first(head_, [id](Widget& w) { w.timer(id); });
    return {};
}

auto Application::handle_paint(Canvas canvas) const -> Terminal::Cursor
{
    auto cursor = Terminal::Cursor{std::nullopt};
    ::send_paint_events(head_, canvas, cursor);
    return cursor;
}

}  // namespace ox