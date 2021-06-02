#include <memory>
#include <termox/system/detail/send.hpp>

#include <cassert>

#include <esc/event.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/detail/is_paintable.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/detail/focus.hpp>
#include <termox/system/event.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/detail/screen_buffers.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/widget.hpp>

namespace {

/// Sends delete event, emit signal, disables animation and maybe clears focus.
void do_delete(ox::Widget& w)
{
    w.delete_event();
    w.deleted.emit();
    w.disable_animation();
    if (ox::detail::Focus::focus_widget() == std::addressof(w))
        ox::detail::Focus::clear_without_posting_event();
}

}  // namespace

namespace ox::detail {

void send(ox::Paint_event e)
{
    if (!is_paintable(e.receiver))
        return;
    auto p = Painter{e.receiver, ox::Terminal::screen_buffers.next};
    e.receiver.get().paint_event(p);
    e.receiver.get().painted.emit();
}

void send(ox::Key_press_event e)
{
    if (e.receiver) {
        e.receiver->get().key_press_event(e.key);
        e.receiver->get().key_pressed.emit(e.key);
    }
}

void send(ox::Mouse_press_event e)
{
    detail::Focus::mouse_press(e.receiver);
    e.receiver.get().mouse_press_event(e.data);
    e.receiver.get().mouse_pressed.emit(e.data);
}

void send(ox::Mouse_release_event e)
{
    e.receiver.get().mouse_release_event(e.data);
    e.receiver.get().mouse_released.emit(e.data);
}

void send(ox::Mouse_wheel_event e)
{
    e.receiver.get().mouse_wheel_event(e.data);
    e.receiver.get().mouse_wheel_scrolled.emit(e.data);
}

void send(ox::Mouse_move_event e)
{
    e.receiver.get().mouse_move_event(e.data);
    e.receiver.get().mouse_moved.emit(e.data);
}

void send(ox::Child_added_event e)
{
    e.receiver.get().child_added_event(e.child.get());
    e.receiver.get().child_added.emit(e.child.get());
}

void send(ox::Child_removed_event e)
{
    e.receiver.get().child_removed_event(e.child.get());
    e.receiver.get().child_removed.emit(e.child.get());
}

void send(ox::Child_polished_event e)
{
    e.receiver.get().child_polished_event(e.child.get());
    e.receiver.get().child_polished.emit(e.child.get());
}

void send(ox::Delete_event e)
{
    if (e.removed == nullptr)
        return;
    do_delete(*e.removed);
    for (Widget* w : e.removed->get_descendants())
        do_delete(*w);
    e.removed.reset();
}

void send(ox::Disable_event e)
{
    e.receiver.get().disable_event();
    e.receiver.get().disabled.emit();
}

void send(ox::Enable_event e)
{
    e.receiver.get().enable_event();
    e.receiver.get().enabled.emit();
}

void send(ox::Focus_in_event e)
{
    e.receiver.get().focus_in_event();
    e.receiver.get().focused_in.emit();
}

void send(ox::Focus_out_event e)
{
    e.receiver.get().focus_out_event();
    e.receiver.get().focused_out.emit();
}

void send(ox::Move_event e)
{
    auto const previous = e.receiver.get().top_left();
    if (previous == e.new_position)
        return;
    e.receiver.get().set_top_left(e.new_position);
    e.receiver.get().move_event(e.new_position, previous);
    e.receiver.get().moved.emit(e.new_position, previous);
}

void send(ox::Resize_event e)
{
    auto const previous = e.receiver.get().area();
    if (previous == e.new_area)
        return;
    e.receiver.get().set_area(e.new_area);
    e.receiver.get().resize_event(e.new_area, previous);
    e.receiver.get().resized.emit(e.new_area, previous);
}

void send(ox::Timer_event e)
{
    if (e.receiver.get().is_enabled()) {
        e.receiver.get().timer_event();
        e.receiver.get().timer.emit();
    }
}

void send(ox::Dynamic_color_event const& e)
{
    for (auto [color, true_color] : e.color_data) {
        ox::Terminal::update_color_stores(color, true_color);
        ox::Terminal::repaint_color(color);
    }
}

void send(::esc::Window_resize x)
{
    ox::Widget& head = []() -> ox::Widget& {
        ox::Widget* head = ox::System::head();
        assert(head != nullptr);
        return *head;
    }();
    auto const previous   = ox::Terminal::screen_buffers.area();
    auto const is_shorter = x.new_dimensions.height < previous.height;
    if (is_shorter)  // xterm anchors from the top and scrolls.
        ox::Terminal::flag_full_repaint();
    ox::Terminal::screen_buffers.resize(x.new_dimensions);
    ox::System::post_event(ox::Resize_event{head, x.new_dimensions});
}

void send(ox::Custom_event e) { e.send(); }

}  // namespace ox::detail
