#include <cppurses/widget/widget_slots.hpp>

#include <signals/slot.hpp>

#include <cppurses/system/events/close_event.hpp>
#include <cppurses/system/events/deferred_delete_event.hpp>
#include <cppurses/system/events/hide_event.hpp>
#include <cppurses/system/events/key_event.hpp>
#include <cppurses/system/events/mouse_event.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/events/show_event.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace slot {

sig::Slot<void()> delete_later(Widget& w) {
    sig::Slot<void()> slot{
        [&w] { System::post_event<Deferred_delete_event>(&w); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> close(Widget& w) {
    sig::Slot<void()> slot{[&w] { System::post_event<Close_event>(&w); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> hide(Widget& w) {
    sig::Slot<void()> slot{[&w] { System::post_event<Hide_event>(&w); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> show(Widget& w) {
    sig::Slot<void()> slot{[&w] { System::post_event<Show_event>(&w); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> repaint(Widget& w) {
    sig::Slot<void()> slot{[&w] { System::send_event(Paint_event(&w)); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> update(Widget& w) {
    sig::Slot<void()> slot{[&w] { w.update(); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void(Point, Mouse_button)> click(Widget& w) {
    sig::Slot<void(Point, Mouse_button)> slot{
        [&w](const Point& c, Mouse_button b) {
            System::send_event(Mouse_press_event{
                &w, b, Point{w.x() + c.x, w.y() + c.y}, c, 0});
        }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void(Mouse_button)> click(Widget& w, Point c) {
    sig::Slot<void(Mouse_button)> slot{[&w, &c](Mouse_button b) {
        System::send_event(
            Mouse_press_event{&w, b, Point{w.x() + c.x, w.y() + c.y}, c, 0});
    }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void(Point)> click(Widget& w, Mouse_button b) {
    sig::Slot<void(Point)> slot{[&w, b](const Point& c) {
        System::send_event(
            Mouse_press_event{&w, b, Point{w.x() + c.x, w.y() + c.y}, c, 0});
    }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> click(Widget& w, Point c, Mouse_button b) {
    sig::Slot<void()> slot{[&w, &c, b] {
        System::send_event(
            Mouse_press_event{&w, b, Point{w.x() + c.x, w.y() + c.y}, c, 0});
    }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void(Key)> keypress(Widget& w) {
    sig::Slot<void(Key)> slot{[&w](Key k) {
        System::send_event(Key_press_event{&w, k});
    }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> keypress(Widget& w, Key k) {
    sig::Slot<void()> slot{[&w, k] {
        System::send_event(Key_press_event{&w, k});
    }};
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
    sig::Slot<void()> slot{[&w] { cppurses::toggle_cursor(w); }};
    slot.track(w.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
