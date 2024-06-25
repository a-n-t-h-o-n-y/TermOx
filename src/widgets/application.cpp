#include <termox/widgets/application.hpp>

#include <algorithm>

#include <termox/widgets/widget.hpp>

namespace {
using namespace ox::widgets;

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

auto send_leave_events(Widget& w, ox::Point p) -> void
{
    mouse_leave(w);
    Widget* const next = find_widget_at(children(w), p);
    if (next != nullptr) {
        send_leave_events(*next, p - next->at);
    }
}

auto send_enter_events(Widget& w, ox::Point p) -> void
{
    mouse_enter(w);
    Widget* const next = find_widget_at(children(w), p);
    if (next != nullptr) {
        send_enter_events(*next, p - next->at);
    }
}

auto send_enter_leave_events(Widget& w, ox::Point previous, ox::Point current) -> void
{
    Widget* const previous_widget = find_widget_at(children(w), previous);
    Widget* const current_widget = find_widget_at(children(w), current);

    if (previous_widget != nullptr && current_widget != nullptr &&
        previous != current) {
        send_leave_events(*previous_widget, previous - previous_widget->at);
        send_enter_events(*current_widget, current - current_widget->at);
    }
}

enum class SetFocus : bool { Yes, No };

template <SetFocus SF, typename EventFn>
auto any_mouse_event(Widget& head, ox::Mouse m, EventFn&& event_fn) -> void
{
    Widget* const found_ptr = find_widget_at(children(head), m.at);

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

auto send_paint_events(Widget const& head, ox::Canvas canvas) -> void
{
    if (head.enabled && head.size.width > 0 && head.size.height > 0) {
        paint(head, canvas);
    }
    for (auto const& child : children(head)) {
        send_paint_events(child, ox::Canvas{
                                     .buffer = canvas.buffer,
                                     .at = canvas.at + child.at,
                                     .size = child.size,
                                 });
    }
}

}  // namespace

namespace ox::widgets {

auto Application::run() -> int { return process_events(term_, *this); }

auto Application::handle_mouse_press(ox::Mouse m) -> ox::EventResponse
{
    ::any_mouse_event<SetFocus::Yes>(head_, m,
                                     [](Widget& w, ox::Mouse m) { mouse_press(w, m); });
    return {};
}

auto Application::handle_mouse_release(ox::Mouse m) -> ox::EventResponse
{
    ::any_mouse_event<SetFocus::No>(
        head_, m, [](Widget& w, ox::Mouse m) { mouse_release(w, m); });
    return {};
}

auto Application::handle_mouse_wheel(ox::Mouse m) -> ox::EventResponse
{
    ::any_mouse_event<SetFocus::No>(head_, m,
                                    [](Widget& w, ox::Mouse m) { mouse_wheel(w, m); });
    return {};
}

auto Application::handle_mouse_move(ox::Mouse m) -> ox::EventResponse
{
    send_enter_leave_events(head_, previous_mouse_position_, m.at);
    ::any_mouse_event<SetFocus::No>(head_, m,
                                    [](Widget& w, ox::Mouse m) { mouse_move(w, m); });
    previous_mouse_position_ = m.at;
    return {};
}

auto Application::handle_key_press(ox::Key k) -> ox::EventResponse
{
    // TODO a shortcuts manager? processed here?
    Widget* const focused = Focus::get();
    if (focused != nullptr) {
        if (focused->focus_policy == FocusPolicy::Strong ||
            focused->focus_policy == FocusPolicy::Tab) {
            if (k == ox::Key::Tab) {
                do_tab_focus_change(head_, focused);
            }
            else if (k == ox::Key::BackTab) {
                do_shift_tab_focus_change(head_, focused);
            }
            else {
                key_press(*focused, k);
            }
        }
        else {
            key_press(*focused, k);
        }
    }
    return {};
}

auto Application::handle_key_release(ox::Key k) -> ox::EventResponse
{
    Widget* const focused = Focus::get();
    if (focused != nullptr) {
        key_release(*focused, k);
    }
    return {};
}

auto Application::handle_resize(ox::Area new_size) -> ox::EventResponse
{
    resize(head_, new_size);
    return {};
}

auto Application::handle_timer(int id) -> ox::EventResponse
{
    for_each_depth_first(head_, [id](Widget& w) { timer(w, id); });
    return {};
}

auto Application::handle_cursor() -> ox::Terminal::Cursor
{
    Widget* const focused = Focus::get();
    // TODO
    // you need to get the offset to apply to the returned cursor point if it is not
    // nullopt. If focus is an in tree implementation you can do this, otherwise it is
    // not straightforward.
    return focused != nullptr ? cursor(*focused) : std::nullopt;
}

auto Application::handle_paint(ox::Canvas canvas) const -> void
{
    ::send_paint_events(head_, canvas);
}

}  // namespace ox::widgets