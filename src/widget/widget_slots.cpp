#include "widget/widget_slots.hpp"
#include "system/events/deferred_delete_event.hpp"
#include "system/events/close_event.hpp"
#include "system/events/hide_event.hpp"
#include "system/events/mouse_event.hpp"
#include "system/events/paint_event.hpp"
#include "system/events/key_event.hpp"
#include "system/events/show_event.hpp"
#include "system/system.hpp"
#include "system/mouse_button.hpp"
#include "widget/coordinates.hpp"
#include "widget/widget.hpp"
#include <signals/slot.hpp>
#include <functional>

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

sig::Slot<void(Coordinates, Mouse_button)> click(Widget& w) {
    sig::Slot<void(Coordinates, Mouse_button)> slot{
        [&w](const Coordinates& c, Mouse_button b) {
            System::send_event(Mouse_press_event{&w, b, w.x() + c.x,
                                                 w.y() + c.y, c.x, c.y, 0});
        }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void(Mouse_button)> click(Widget& w, Coordinates c) {
    sig::Slot<void(Mouse_button)> slot{[&w, &c](Mouse_button b) {
        System::send_event(
            Mouse_press_event{&w, b, w.x() + c.x, w.y() + c.y, c.x, c.y, 0});
    }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void(Coordinates)> click(Widget& w, Mouse_button b) {
    sig::Slot<void(Coordinates)> slot{[&w, b](const Coordinates& c) {
        System::send_event(
            Mouse_press_event{&w, b, w.x() + c.x, w.y() + c.y, c.x, c.y, 0});
    }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void()> click(Widget& w, Coordinates c, Mouse_button b) {
    sig::Slot<void()> slot{[&w, &c, b] {
        System::send_event(
            Mouse_press_event{&w, b, w.x() + c.x, w.y() + c.y, c.x, c.y, 0});
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

}  // namespace slot
}  // namespace cppurses
