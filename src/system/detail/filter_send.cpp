#include <termox/system/detail/filter_send.hpp>

#include <algorithm>
#include <iterator>
#include <set>

#include <termox/painter/detail/is_paintable.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/detail/focus.hpp>
#include <termox/system/event.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/widget.hpp>

namespace {

/// Applies \p filter_function over \p filters, up until it returns true.
/** If none return true, then this returns false. */
template <typename F>
auto apply_until_accepted(F&& filter_function,
                          std::set<ox::Widget*> const& filters) -> bool
{
    return std::find_if(std::begin(filters), std::end(filters),
                        filter_function) != std::end(filters);
}

}  // namespace

namespace ox::detail {

auto filter_send(ox::Paint_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            if (!is_paintable(e.receiver))
                return false;
            auto p = Painter{e.receiver, ox::Terminal::screen_buffers.next};
            auto const x = filter->paint_event_filter(e.receiver, p);
            auto const y = filter->painted_filter.emit(e.receiver, p);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Key_press_event const& e) -> bool
{
    switch (e.key) {
        case Key::Tab: detail::Focus::tab_press(); break;
        case Key::Back_tab: detail::Focus::shift_tab_press(); break;
        default: break;
    }
    if (!e.receiver)
        return true;
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x =
                filter->key_press_event_filter(e.receiver->get(), e.key);
            auto const y =
                filter->key_pressed_filter.emit(e.receiver->get(), e.key);
            return x || (y ? *y : false);
        },
        e.receiver->get().get_event_filters());
}

auto filter_send(ox::Mouse_press_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x = filter->mouse_press_event_filter(e.receiver, e.data);
            auto const y =
                filter->mouse_pressed_filter.emit(e.receiver, e.data);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Mouse_release_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x =
                filter->mouse_release_event_filter(e.receiver, e.data);
            auto const y =
                filter->mouse_released_filter.emit(e.receiver, e.data);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Mouse_wheel_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x = filter->mouse_wheel_event_filter(e.receiver, e.data);
            auto const y =
                filter->mouse_wheel_scrolled_filter.emit(e.receiver, e.data);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Mouse_move_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x = filter->mouse_move_event_filter(e.receiver, e.data);
            auto const y = filter->mouse_moved_filter.emit(e.receiver, e.data);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Child_added_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x =
                filter->child_added_event_filter(e.receiver, e.child);
            auto const y = filter->child_added_filter.emit(e.receiver, e.child);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Child_removed_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x =
                filter->child_removed_event_filter(e.receiver, e.child);
            auto const y =
                filter->child_removed_filter.emit(e.receiver, e.child);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Child_polished_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x =
                filter->child_polished_event_filter(e.receiver, e.child);
            auto const y =
                filter->child_polished_filter.emit(e.receiver, e.child);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Delete_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x = filter->delete_event_filter(*e.removed);
            auto const y = filter->deleted_filter.emit(*e.removed);
            return x || (y ? *y : false);
        },
        e.removed->get_event_filters());
}

auto filter_send(ox::Disable_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x = filter->disable_event_filter(e.receiver);
            auto const y = filter->disabled_filter.emit(e.receiver);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Enable_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x = filter->enable_event_filter(e.receiver);
            auto const y = filter->enabled_filter.emit(e.receiver);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Focus_in_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x = filter->focus_in_event_filter(e.receiver);
            auto const y = filter->focused_in_filter.emit(e.receiver);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Focus_out_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x = filter->focus_out_event_filter(e.receiver);
            auto const y = filter->focused_out_filter.emit(e.receiver);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Move_event const& e) -> bool
{
    auto const old_position = e.receiver.get().top_left();
    auto const new_position = e.new_position;
    if (old_position == new_position)
        return true;
    return apply_until_accepted(
        [&](Widget* filter) {
            auto const x = filter->move_event_filter(e.receiver, new_position,
                                                     old_position);
            auto const y = filter->moved_filter.emit(e.receiver, new_position,
                                                     old_position);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Resize_event const& e) -> bool
{
    auto const previous = e.receiver.get().area();
    auto const new_area = e.new_area;
    if (previous == new_area)
        return true;
    return apply_until_accepted(
        [&](Widget* filter) {
            auto const x =
                filter->resize_event_filter(e.receiver, new_area, previous);
            auto const y =
                filter->resized_filter.emit(e.receiver, new_area, previous);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Timer_event const& e) -> bool
{
    return apply_until_accepted(
        [&e](Widget* filter) {
            auto const x = filter->timer_event_filter(e.receiver);
            auto const y = filter->timer_filter.emit(e.receiver);
            return x || (y ? *y : false);
        },
        e.receiver.get().get_event_filters());
}

auto filter_send(ox::Dynamic_color_event const&) -> bool { return false; }

auto filter_send(::esc::Window_resize) -> bool { return false; }

auto filter_send(ox::Custom_event const& e) -> bool { return e.filter_send(); }

}  // namespace ox::detail
