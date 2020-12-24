#include <termox/widget/widget_slots.hpp>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/system/event.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/cursor.hpp>
#include <termox/widget/detail/link_lifetimes.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace ox::slot {

auto enable(Widget& w) -> sl::Slot<void()>
{
    return link_lifetimes([&w] { w.enable(); }, w);
}

auto disable(Widget& w) -> sl::Slot<void()>
{
    return link_lifetimes([&w] { w.disable(); }, w);
}

auto update(Widget& w) -> sl::Slot<void()>
{
    return link_lifetimes([&w] { w.update(); }, w);
}

auto click(Widget& w) -> sl::Slot<void(Point, Mouse::Button)>
{
    return link_lifetimes(
        [&w](Point const& p, Mouse::Button b) {
            System::send_event(Mouse_press_event{
                w, Mouse{{w.inner_x() + p.x, w.inner_y() + p.y}, p, b, 0, {}}});
        },
        w);
}

auto click(Widget& w, Point p) -> sl::Slot<void(Mouse::Button)>
{
    return link_lifetimes(
        [&w, p](Mouse::Button b) {
            System::send_event(Mouse_press_event{
                w, Mouse{{w.inner_x() + p.x, w.inner_y() + p.y}, p, b, 0, {}}});
        },
        w);
}

auto click(Widget& w, Mouse::Button b) -> sl::Slot<void(Point)>
{
    return link_lifetimes(
        [&w, b](Point p) {
            System::send_event(Mouse_press_event{
                w, Mouse{{w.inner_x() + p.x, w.inner_y() + p.y}, p, b, 0, {}}});
        },
        w);
}

auto click(Widget& w, Point p, Mouse::Button b) -> sl::Slot<void()>
{
    return link_lifetimes(
        [&w, p, b] {
            System::send_event(Mouse_press_event{
                w, Mouse{{w.inner_x() + p.x, w.inner_y() + p.y}, p, b, 0, {}}});
        },
        w);
}

auto keypress(Widget& w) -> sl::Slot<void(Key)>
{
    return link_lifetimes(
        [&w](Key k) {
            System::send_event(Key_press_event{w, k});
        },
        w);
}

auto keypress(Widget& w, Key k) -> sl::Slot<void()>
{
    return link_lifetimes(
        [&w, k] {
            System::send_event(Key_press_event{w, k});
        },
        w);
}

auto set_background(Widget& w) -> sl::Slot<void(Color)>
{
    return link_lifetimes(
        [&w](Color c) {
            w.brush.background = c;
            w.update();
        },
        w);
}

auto set_background(Widget& w, Color c) -> sl::Slot<void()>
{
    return link_lifetimes(
        [&w, c] {
            w.brush.background = c;
            w.update();
        },
        w);
}

auto set_foreground(Widget& w) -> sl::Slot<void(Color)>
{
    return link_lifetimes(
        [&w](Color c) {
            w.brush.foreground = c;
            w.update();
        },
        w);
}

auto set_foreground(Widget& w, Color c) -> sl::Slot<void()>
{
    return link_lifetimes(
        [&w, c] {
            w.brush.foreground = c;
            w.update();
        },
        w);
}

auto toggle_cursor(Widget& w) -> sl::Slot<void()>
{
    return link_lifetimes([&w] { w.cursor.toggle(); }, w);
}

}  // namespace ox::slot
