#include <cppurses/widget/widget.hpp>

#include <cstdint>

#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {

// - - - - - - - - - - - - - - Event Handlers - - - - - - - - - - - - - - - - -
auto Widget::enable_event() -> bool { return true; }

auto Widget::disable_event() -> bool { return true; }

auto Widget::child_added_event(Widget& child) -> bool
{
    child_added(&child);
    this->update();
    return true;
}

auto Widget::child_removed_event(Widget& child) -> bool
{
    child_removed(&child);
    this->update();
    return true;
}

auto Widget::child_polished_event(Widget&) -> bool
{
    this->update();
    return true;
}

auto Widget::move_event(Point new_position, Point) -> bool
{
    moved(new_position);
    this->update();
    return true;
}

auto Widget::resize_event(Area, Area) -> bool
{
    resized(outer_width_, outer_height_);
    this->update();
    return true;
}

auto Widget::mouse_press_event(Mouse::State const& mouse) -> bool
{
    clicked(mouse.local);
    clicked_xy(mouse.local.x, mouse.local.y);
    return true;
}

auto Widget::mouse_release_event(Mouse::State const& mouse) -> bool
{
    click_released(mouse.global);
    click_released_xy(mouse.global.x, mouse.global.y);
    return true;
}

auto Widget::mouse_double_click_event(Mouse::State const& mouse) -> bool
{
    double_clicked(mouse.global);
    double_clicked_xy(mouse.global.x, mouse.global.y);
    return true;
}

auto Widget::mouse_wheel_event(Mouse::State const&) -> bool { return false; }

auto Widget::mouse_move_event(Mouse::State const&) -> bool { return false; }

auto Widget::key_press_event(Key::State const& keyboard) -> bool
{
    key_pressed(keyboard.key);
    return true;
}

auto Widget::key_release_event(Key::State const& keyboard) -> bool
{
    key_released(keyboard.key);
    return false;
}

auto Widget::focus_in_event() -> bool
{
    focused_in();
    return true;
}

auto Widget::focus_out_event() -> bool
{
    focused_out();
    return true;
}

auto Widget::delete_event() -> bool { return true; }

auto Widget::paint_event() -> bool
{
    Painter p{*this};
    p.border();
    return true;
}

auto Widget::timer_event() -> bool
{
    this->update();
    return true;
}

// - - - - - - - - - - - - Event Filter Handlers - - - - - - - - - - - - - - - -

auto Widget::child_added_event_filter(Widget&, Widget&) -> bool
{
    return false;
}

auto Widget::child_removed_event_filter(Widget&, Widget&) -> bool
{
    return false;
}

auto Widget::child_polished_event_filter(Widget&, Widget&) -> bool
{
    return false;
}

auto Widget::enable_event_filter(Widget&) -> bool { return false; }

auto Widget::disable_event_filter(Widget&) -> bool { return false; }

auto Widget::move_event_filter(Widget&, Point, Point) -> bool { return false; }

auto Widget::resize_event_filter(Widget&, Area, Area) -> bool { return false; }

auto Widget::mouse_press_event_filter(Widget&, Mouse::State const&) -> bool
{
    return false;
}

auto Widget::mouse_release_event_filter(Widget&, Mouse::State const&) -> bool
{
    return false;
}

auto Widget::mouse_double_click_event_filter(Widget&, Mouse::State const&)
    -> bool
{
    return false;
}

auto Widget::mouse_wheel_event_filter(Widget&, Mouse::State const&) -> bool
{
    return false;
}

auto Widget::mouse_move_event_filter(Widget&, Mouse::State const&) -> bool
{
    return false;
}

auto Widget::key_press_event_filter(Widget&, Key::State const&) -> bool
{
    return false;
}

auto Widget::key_release_event_filter(Widget&, Key::State const&) -> bool
{
    return false;
}

auto Widget::focus_in_event_filter(Widget&) -> bool { return false; }

auto Widget::focus_out_event_filter(Widget&) -> bool { return false; }

auto Widget::delete_event_filter(Widget&) -> bool { return false; }

auto Widget::paint_event_filter(Widget&) -> bool { return false; }

auto Widget::timer_event_filter(Widget&) -> bool { return false; }

}  // namespace cppurses
