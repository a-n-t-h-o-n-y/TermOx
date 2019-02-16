#include <cppurses/widget/widget_slots.hpp>

#include <signals/slot.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/cursor_data.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widget_free_functions.hpp>

namespace cppurses {
namespace slot {

sig::Slot<void()> enable(Widget& w) {
    sig::Slot<void()> slot{[&w] { w.enable(); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> disable(Widget& w) {
    sig::Slot<void()> slot{[&w] { w.disable(); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> delete_later(Widget& w) {
    sig::Slot<void()> slot{[&w] { w.close(); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> update(Widget& w) {
    sig::Slot<void()> slot{[&w] { w.update(); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void(Point, Mouse::Button)> click(Widget& w) {
    sig::Slot<void(Point, Mouse::Button)> slot{
        [&w](const Point& c, Mouse::Button b) {
            System::send_event(Mouse::Press{
                w, Mouse::State{b, Point{w.inner_x() + c.x, w.inner_y() + c.y},
                                c, 0}});
        }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void(Mouse::Button)> click(Widget& w, Point c) {
    sig::Slot<void(Mouse::Button)> slot{[&w, &c](Mouse::Button b) {
        System::send_event(Mouse::Press{
            w, Mouse::State{b, Point{w.inner_x() + c.x, w.inner_y() + c.y}, c,
                            0}});
    }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void(Point)> click(Widget& w, Mouse::Button b) {
    sig::Slot<void(Point)> slot{[&w, b](const Point& c) {
        System::send_event(Mouse::Press{
            w, Mouse::State{b, Point{w.inner_x() + c.x, w.inner_y() + c.y}, c,
                            0}});
    }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> click(Widget& w, Point c, Mouse::Button b) {
    sig::Slot<void()> slot{[&w, &c, b] {
        System::send_event(Mouse::Press{
            w, Mouse::State{b, Point{w.inner_x() + c.x, w.inner_y() + c.y}, c,
                            0}});
    }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void(Key::Code)> keypress(Widget& w) {
    sig::Slot<void(Key::Code)> slot{[&w](Key::Code k) {
        System::send_event(Key::Press{w, k});
    }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> keypress(Widget& w, Key::Code k) {
    sig::Slot<void()> slot{[&w, k] { System::send_event(Key::Press{w, k}); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void(Color)> set_background(Widget& w) {
    sig::Slot<void(Color)> slot{
        [&w](Color c) { cppurses::set_background(w, c); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> set_background(Widget& w, Color c) {
    sig::Slot<void()> slot{[&w, &c] { cppurses::set_background(w, c); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void(Color)> set_foreground(Widget& w) {
    sig::Slot<void(Color)> slot{
        [&w](Color c) { cppurses::set_foreground(w, c); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> set_foreground(Widget& w, Color c) {
    sig::Slot<void()> slot{[&w, &c] { cppurses::set_foreground(w, c); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> toggle_cursor(Widget& w) {
    sig::Slot<void()> slot{[&w] { w.cursor.toggle(); }};
    slot.track(w.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
