#ifndef TERMOX_SYSTEM_DETAIL_SEND_HPP
#define TERMOX_SYSTEM_DETAIL_SEND_HPP
#include <esc/event.hpp>

#include <termox/painter/detail/is_paintable.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/detail/focus.hpp>
#include <termox/system/event.hpp>
#include <termox/system/key.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/widget.hpp>

namespace ox::detail {

inline void send(ox::Paint_event e)
{
    if (is_paintable(e.receiver)) {
        auto p = Painter{e.receiver, Terminal::screen_buffers.next};
        if (!e.receiver.get().is_layout_type())
            p.wallpaper_fill();
        e.receiver.get().paint_event(p);
    }
}

inline void send(ox::Key_press_event e)
{
    if (e.receiver)
        e.receiver->get().key_press_event(e.key);
}

inline void send(ox::Mouse_press_event e)
{
    detail::Focus::mouse_press(e.receiver);
    e.receiver.get().mouse_press_event(e.data);
}

inline void send(ox::Mouse_release_event e)
{
    e.receiver.get().mouse_release_event(e.data);
}

inline void send(ox::Mouse_wheel_event e)
{
    e.receiver.get().mouse_wheel_event(e.data);
}

inline void send(ox::Mouse_move_event e)
{
    e.receiver.get().mouse_move_event(e.data);
}

inline void send(ox::Child_added_event e)
{
    e.receiver.get().child_added_event(e.child);
}

inline void send(ox::Child_removed_event e)
{
    e.receiver.get().child_removed_event(e.child);
}

inline void send(ox::Child_polished_event e)
{
    e.receiver.get().child_polished_event(e.child);
}

inline void send(ox::Delete_event e)
{
    if (e.removed == nullptr)
        return;
    e.removed->delete_event();
    for (Widget* w : e.removed->get_descendants())
        w->delete_event();
    e.removed.reset();
}

inline void send(ox::Disable_event e) { e.receiver.get().disable_event(); }

inline void send(ox::Enable_event e) { e.receiver.get().enable_event(); }

inline void send(ox::Focus_in_event e)
{
    Focus::direct_set_focus(e.receiver.get());
    e.receiver.get().focus_in_event();
}

inline void send(ox::Focus_out_event e) { e.receiver.get().focus_out_event(); }

inline void send(ox::Move_event e)
{
    auto const old_position = e.receiver.get().top_left();
    auto const new_position = e.new_position;
    if (old_position == new_position)
        return;
    e.receiver.get().set_top_left(new_position);
    e.receiver.get().move_event(new_position, old_position);
}

inline void send(ox::Resize_event e)
{
    auto const old_area = e.receiver.get().outer_area();
    auto const new_area = e.new_area;
    if (old_area == new_area)
        return;
    e.receiver.get().set_outer_area(new_area);
    e.receiver.get().resize_event(new_area, old_area);
}

inline void send(ox::Timer_event e)
{
    if (e.receiver.get().is_enabled())
        e.receiver.get().timer_event();
}

inline void send(ox::Dynamic_color_event const& e)
{
    for (auto [color, true_color] : e.color_data) {
        ox::Terminal::update_color_stores(color, true_color);
        ox::Terminal::repaint_color(color);
    }
}

/// Modify Screen_buffers object and post event to head Widget.
inline void send(::esc::Window_resize x)
{
    ox::Widget& head = []() -> ox::Widget& {
        ox::Widget* head = ox::System::head();
        assert(head != nullptr);
        return *head;
    }();
    ox::Terminal::flag_full_repaint();
    ox::Terminal::screen_buffers.resize(x.new_dimensions);
    System::post_event(ox::Resize_event{head, x.new_dimensions});
}

inline void send(ox::Custom_event e) { e.send(); }

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_SEND_HPP
