#include <cppurses/widget/widget_slots.hpp>

#include <signals/slot.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/cursor_data.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace slot {

auto enable(Widget& w) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&w] { w.enable(); }};
    slot.track(w.destroyed);
    return slot;
}

auto disable(Widget& w) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&w] { w.disable(); }};
    slot.track(w.destroyed);
    return slot;
}

auto delete_later(Widget& w) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&w] { w.close(); }};
    slot.track(w.destroyed);
    return slot;
}

auto update(Widget& w) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&w] { w.update(); }};
    slot.track(w.destroyed);
    return slot;
}

auto click(Widget& w) -> sig::Slot<void(Point, Mouse::Button)>
{
    auto slot = sig::Slot<void(Point, Mouse::Button)>{
        [&w](const Point& p, Mouse::Button b) {
            System::send_event(Mouse::Press{
                w, Mouse::State{b, Point{w.inner_x() + p.x, w.inner_y() + p.y},
                                p, 0}});
        }};
    slot.track(w.destroyed);
    return slot;
}

auto click(Widget& w, Point p) -> sig::Slot<void(Mouse::Button)>
{
    auto slot = sig::Slot<void(Mouse::Button)>{[&w, p](Mouse::Button b) {
        System::send_event(Mouse::Press{
            w, Mouse::State{b, Point{w.inner_x() + p.x, w.inner_y() + p.y}, p,
                            0}});
    }};
    slot.track(w.destroyed);
    return slot;
}

auto click(Widget& w, Mouse::Button b) -> sig::Slot<void(Point)>
{
    auto slot = sig::Slot<void(Point)>{[&w, b](Point p) {
        System::send_event(Mouse::Press{
            w, Mouse::State{b, Point{w.inner_x() + p.x, w.inner_y() + p.y}, p,
                            0}});
    }};
    slot.track(w.destroyed);
    return slot;
}

auto click(Widget& w, Point p, Mouse::Button b) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&w, p, b] {
        System::send_event(Mouse::Press{
            w, Mouse::State{b, Point{w.inner_x() + p.x, w.inner_y() + p.y}, p,
                            0}});
    }};
    slot.track(w.destroyed);
    return slot;
}

auto keypress(Widget& w) -> sig::Slot<void(Key::Code)>
{
    auto slot = sig::Slot<void(Key::Code)>{[&w](Key::Code k) {
        System::send_event(Key::Press{w, k});
    }};
    slot.track(w.destroyed);
    return slot;
}

auto keypress(Widget& w, Key::Code k) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&w, k] {
        System::send_event(Key::Press{w, k});
    }};
    slot.track(w.destroyed);
    return slot;
}

auto set_background(Widget& w) -> sig::Slot<void(Color)>
{
    auto slot = sig::Slot<void(Color)>{[&w](Color c) {
        w.brush.set_background(c);
        w.update();
    }};
    slot.track(w.destroyed);
    return slot;
}

auto set_background(Widget& w, Color c) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&w, c] {
        w.brush.set_background(c);
        w.update();
    }};
    slot.track(w.destroyed);
    return slot;
}

auto set_foreground(Widget& w) -> sig::Slot<void(Color)>
{
    auto slot = sig::Slot<void(Color)>{[&w](Color c) {
        w.brush.set_foreground(c);
        w.update();
    }};
    slot.track(w.destroyed);
    return slot;
}

auto set_foreground(Widget& w, Color c) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&w, c] {
        w.brush.set_foreground(c);
        w.update();
    }};
    slot.track(w.destroyed);
    return slot;
}

auto toggle_cursor(Widget& w) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&w] { w.cursor.toggle(); }};
    slot.track(w.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
