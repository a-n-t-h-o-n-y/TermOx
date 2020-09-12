#ifndef CPPURSES_SYSTEM_DETAIL_SEND_HPP
#define CPPURSES_SYSTEM_DETAIL_SEND_HPP
#include <cppurses/painter/detail/is_paintable.hpp>
#include <cppurses/system/detail/focus.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses::detail {

inline void send(cppurses::Paint_event e)
{
    if (is_paintable(e.receiver))
        e.receiver.get().paint_event();
}

inline void send(cppurses::Key_press_event e)
{
    e.receiver.get().key_press_event(e.key);
    switch (e.key) {
        case Key::Tab: detail::Focus::tab_press(); break;
        case Key::Back_tab: detail::Focus::shift_tab_press(); break;
        default: break;
    }
}

inline void send(cppurses::Mouse_press_event e)
{
    detail::Focus::mouse_press(e.receiver);
    e.receiver.get().mouse_press_event(e.data);
}

inline void send(cppurses::Mouse_release_event e)
{
    e.receiver.get().mouse_release_event(e.data);
}

inline void send(cppurses::Mouse_double_click_event e)
{
    e.receiver.get().mouse_double_click_event(e.data);
}

inline void send(cppurses::Mouse_wheel_event e)
{
    e.receiver.get().mouse_wheel_event(e.data);
}

inline void send(cppurses::Mouse_move_event e)
{
    e.receiver.get().mouse_move_event(e.data);
}

inline void send(cppurses::Child_added_event e)
{
    e.receiver.get().child_added_event(e.child);
}

inline void send(cppurses::Child_removed_event e)
{
    e.receiver.get().child_removed_event(e.child);
}

inline void send(cppurses::Child_polished_event e)
{
    e.receiver.get().child_polished_event(e.child);
}

inline void send(cppurses::Delete_event e)
{
    if (e.removed == nullptr)
        return;
    e.removed->delete_event();
    for (Widget* w : e.removed->get_descendants())
        w->delete_event();
    e.removed.reset();
}

inline void send(cppurses::Disable_event e)
{
    e.receiver.get().screen_state().clear();
    e.receiver.get().disable_event();
}

inline void send(cppurses::Enable_event e) { e.receiver.get().enable_event(); }

inline void send(cppurses::Focus_in_event e)
{
    Focus::direct_set_focus(e.receiver.get());
    e.receiver.get().focus_in_event();
}

inline void send(cppurses::Focus_out_event e)
{
    e.receiver.get().focus_out_event();
}

inline void send(cppurses::Move_event e)
{
    auto const old_position = e.receiver.get().top_left();
    auto const new_position = e.new_position;
    if (old_position == new_position)
        return;
    e.receiver.get().screen_state().clear();
    e.receiver.get().screen_state().move(new_position);
    e.receiver.get().set_top_left(new_position);
    e.receiver.get().move_event(new_position, old_position);
}

inline void send(cppurses::Resize_event e)
{
    auto const old_area = e.receiver.get().outer_area();
    auto const new_area = e.new_area;
    if (old_area == new_area)
        return;
    e.receiver.get().set_outer_area(new_area);
    e.receiver.get().screen_state().resize(new_area);
    e.receiver.get().resize_event(new_area, old_area);
}

inline void send(cppurses::Timer_event e)
{
    if (detail::is_paintable(e.receiver))
        e.receiver.get().timer_event();
}

inline void send(cppurses::Custom_event e) { e.send(); }

}  // namespace cppurses::detail
#endif  // CPPURSES_SYSTEM_DETAIL_SEND_HPP
