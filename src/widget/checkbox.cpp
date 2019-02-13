#include <cppurses/widget/widgets/checkbox.hpp>

#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/size_policy.hpp>

namespace cppurses {

Checkbox::Checkbox(Glyph_string label, int padding)
    : label_{std::move(label)}, padding_{padding} {
    this->set_name("Checkbox");
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
}

void Checkbox::toggle() {
    checked_ = !checked_;
    toggled();
    checked_ ? checked() : unchecked();
    this->update();
}

void Checkbox::check() {
    if (checked_) {
        return;
    }
    this->toggle();
}

void Checkbox::uncheck() {
    if (!checked_) {
        return;
    }
    this->toggle();
}

bool Checkbox::is_checked() const {
    return checked_;
}

void Checkbox::set_check_look(const Glyph& symbol) {
    checked_box_ = symbol;
    this->update();
}

void Checkbox::set_uncheck_look(const Glyph& symbol) {
    unchecked_box_ = symbol;
    this->update();
}

const Glyph& Checkbox::check_look() const {
    return checked_box_;
}

const Glyph& Checkbox::uncheck_look() const {
    return unchecked_box_;
}

const Glyph_string& Checkbox::label() const {
    return label_;
}

bool Checkbox::paint_event() {
    Painter p{*this};
    if (this->is_checked()) {
        p.put(checked_box_, 0, 0);
    } else {
        p.put(unchecked_box_, 0, 0);
    }
    p.put(label_, padding_, 0);
    return Widget::paint_event();
}

bool Checkbox::mouse_press_event(const Mouse_data& mouse) {
    if (mouse.button == Mouse_button::Left) {
        this->toggle();
    }
    return Widget::mouse_press_event(mouse);
}

namespace slot {

sig::Slot<void()> toggle(Checkbox& cb) {
    sig::Slot<void()> slot{[&cb]() { cb.toggle(); }};
    slot.track(cb.destroyed);
    return slot;
}

sig::Slot<void()> check(Checkbox& cb) {
    sig::Slot<void()> slot{[&cb]() { cb.check(); }};
    slot.track(cb.destroyed);
    return slot;
}

sig::Slot<void()> uncheck(Checkbox& cb) {
    sig::Slot<void()> slot{[&cb]() { cb.uncheck(); }};
    slot.track(cb.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
