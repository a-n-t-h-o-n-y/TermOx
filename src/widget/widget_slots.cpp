#include "widget/widget_slots.hpp"
#include "system/events/deferred_delete_event.hpp"
#include "system/system.hpp"
#include "widget/widget.hpp"
#include <signals/signals.hpp>
#include <functional>

namespace cppurses {
namespace slot {

sig::Slot<void()> delete_later(Widget& w) {
    sig::Slot<void()> slot{
        [&w] { System::post_event<Deferred_delete_event>(&w); }};
    slot.track(w.destroyed);
    return slot;
}

// sig::Slot<void()> enable(Widget& w) {
//     this->enable = [this] { this->set_enabled(true); };
//     this->enable.track(this->destroyed);
// }

// sig::Slot<void()> disable(Widget& w) {
//     this->disable = [this] { this->set_enabled(false); };
//     this->disable.track(this->destroyed);
// }

// sig::Slot<void()> close(Widget& w) {
//     close = [this]() { System::post_event<Close_event>(this); };
//     close.track(this->destroyed);
// }

// sig::Slot<void()> hide(Widget& w) {
//     hide = [this]() { this->set_visible(false); };
//     hide.track(this->destroyed);
// }

// sig::Slot<void()> show(Widget& w) {
//     show = [this]() { this->set_visible(true); };
//     show.track(this->destroyed);
// }

// sig::Slot<void()> repaint(Widget& w) {  // same as update?
//     repaint = [this]() { System::send_event(Paint_event{this}); };
//     repaint.track(this->destroyed);
// }

// sig::Slot<void()> update(Widget& w) {
//     update_me = [this]() { this->update(); };
//     update_me.track(this->destroyed);
// }

// sig::Slot<void(Mouse_button, Coordinates)> click_me(Widget& w) {
//     click_me = [this](Mouse_button b, Coordinates c) {
//         System::send_event(Mouse_press_event{this, b, this->x() + c.x,
//                                              this->y() + c.y, c.x, c.y, 0});
//     };
//     click_me.track(this->destroyed);
// }

// sig::Slot<void(Key)> keypress_me(Widget& w) {
//     keypress_me = [this](Key key) {
//         System::send_event(Key_press_event{this, key});
//     };
//     keypress_me.track(this->destroyed);
// }

sig::Slot<void(Color)> set_background(Widget& w) {
    sig::Slot<void(Color)> slot{[&w](Color c) { w.set_background(c); }};
    slot.track(w.destroyed);
    return slot;
}

sig::Slot<void(Color)> set_foreground(Widget& w) {
    sig::Slot<void(Color)> slot{[&w](Color c) { w.set_foreground(c); }};
    slot.track(w.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
