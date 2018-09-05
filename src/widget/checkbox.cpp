#include <cppurses/widget/widgets/checkbox.hpp>

#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/widget/size_policy.hpp>

namespace cppurses {

Checkbox::Checkbox(Glyph_string title, int padding)
    : title_{std::move(title)}, padding_{padding} {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
}

void Checkbox::toggle() {
    is_checked_ = !is_checked_;
    toggled();
    is_checked_ ? checked() : unchecked();
    this->update();
}

bool Checkbox::is_checked() const {
    return is_checked_;
}

Glyph_string Checkbox::title() const {
    return title_;
}

bool Checkbox::paint_event() {
    Painter p{this};
    if (this->is_checked()) {
        p.put(checked_box_, 0, 0);
    } else {
        p.put(empty_box_, 0, 0);
    }
    p.put(title_, padding_, 0);
    return Widget::paint_event();
}

bool Checkbox::mouse_press_event(const Mouse_data& mouse) {
    if (mouse.button == Mouse_button::Left) {
        this->toggle();
    }
    return Widget::mouse_press_event(mouse);
}

void check(Checkbox& cb) {
    if (!cb.is_checked()) {
        cb.toggle();
    }
    cb.update();
}

void uncheck(Checkbox& cb) {
    if (cb.is_checked()) {
        cb.toggle();
    }
    cb.update();
}

namespace slot {

sig::Slot<void()> toggle(Checkbox& cb) {
    sig::Slot<void()> slot{[&cb]() { cb.toggle(); }};
    slot.track(cb.destroyed);
    return slot;
}

sig::Slot<void()> check(Checkbox& cb) {
    sig::Slot<void()> slot{[&cb]() { cppurses::check(cb); }};
    slot.track(cb.destroyed);
    return slot;
}

sig::Slot<void()> uncheck(Checkbox& cb) {
    sig::Slot<void()> slot{[&cb]() { cppurses::uncheck(cb); }};
    slot.track(cb.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
