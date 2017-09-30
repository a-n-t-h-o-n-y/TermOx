#include "widget/widgets/textbox.hpp"
#include <cstddef>
#include <iterator>
#include <utility>
#include "painter/glyph_string.hpp"
#include "system/events/key_event.hpp"
#include "system/events/mouse_event.hpp"
#include "system/key.hpp"
#include "widget/coordinates.hpp"
#include "widget/focus_policy.hpp"

namespace cppurses {

Textbox::Textbox(Glyph_string contents) : Textbox_base{std::move(contents)} {
    this->focus_policy = Focus_policy::Strong;
}

void Textbox::enable_scrollwheel(bool enable) {
    scroll_wheel_ = enable;
}

void Textbox::disable_scrollwheel(bool disable) {
    scroll_wheel_ = !disable;
}

void Textbox::toggle_scrollwheel() {
    scroll_wheel_ = !scroll_wheel_;
}

bool Textbox::does_scrollwheel() const {
    return scroll_wheel_;
}

void Textbox::set_wheel_speed(std::size_t lines) {
    this->set_wheel_speed_up(lines);
    this->set_wheel_speed_down(lines);
}

void Textbox::set_wheel_speed_up(std::size_t lines) {
    scroll_speed_up_ = lines;
}

void Textbox::set_wheel_speed_down(std::size_t lines) {
    scroll_speed_down_ = lines;
}

bool Textbox::key_press_event(Key key, char symbol) {
    switch (key) {
        case Key::Backspace: {
            auto cursor_index = this->cursor_index();
            if (cursor_index == 0) {
                break;
            }
            this->erase(--cursor_index, 1);
            if (this->line_at(cursor_index) < this->top_line()) {
                this->scroll_up(1);
            }
            this->set_cursor(cursor_index);
        } break;

        case Key::Enter: {
            auto cursor_index = this->cursor_index();
            this->insert('\n', cursor_index);
            if (this->cursor_y() + 1 == this->height()) {
                this->scroll_down(1);
            }
            this->set_cursor(cursor_index + 1);
        } break;

        case Key::Tab:
            // Insert '\t', it will be taken care of in update_display()
            break;

        case Key::Arrow_right:
            this->cursor_right(1);
            break;

        case Key::Arrow_left:
            this->cursor_left(1);
            break;

        case Key::Arrow_up:
            this->cursor_up(1);
            break;

        case Key::Arrow_down:
            this->cursor_down(1);
            break;

        default:  // Insert text
            char text = symbol;
            if (text != '\0') {
                auto cursor_index = this->cursor_index();
                this->insert(text, cursor_index);
                this->cursor_right(1);
                this->set_cursor(cursor_index + 1);
            }
    }
    this->update();
    return true;
}

bool Textbox::mouse_press_event(Mouse_button button,
                                std::size_t global_x,
                                std::size_t global_y,
                                std::size_t local_x,
                                std::size_t local_y,
                                std::uint8_t device_id) {
    if (button == Mouse_button::Left) {
        this->set_cursor(local_x, local_y);
    } else if (button == Mouse_button::ScrollUp) {
        if (scroll_wheel_) {
            this->scroll_up(scroll_speed_up_);
        }
    } else if (button == Mouse_button::ScrollDown) {
        if (scroll_wheel_) {
            this->scroll_down(scroll_speed_down_);
        }
    }
    this->update();
    return true;
}

namespace slot {

sig::Slot<void()> enable_scrollwheel(Textbox& tb) {
    sig::Slot<void()> slot{[&tb] { tb.enable_scrollwheel(); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> disable_scrollwheel(Textbox& tb) {
    sig::Slot<void()> slot{[&tb] { tb.disable_scrollwheel(); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> toggle_scrollwheel(Textbox& tb) {
    sig::Slot<void()> slot{[&tb] { tb.toggle_scrollwheel(); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> set_scrollwheel(Textbox& tb, bool enable) {
    sig::Slot<void()> slot{[&tb, enable] { tb.enable_scrollwheel(enable); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(bool)> set_scrollwheel(Textbox& tb) {
    sig::Slot<void(bool)> slot{
        [&tb](bool enable) { tb.enable_scrollwheel(enable); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> set_wheel_speed(Textbox& tb, std::size_t lines) {
    sig::Slot<void()> slot{[&tb, lines] { tb.set_wheel_speed(lines); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> set_wheel_speed(Textbox& tb) {
    sig::Slot<void(std::size_t)> slot{
        [&tb](auto lines) { tb.set_wheel_speed(lines); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> set_wheel_speed_up(Textbox& tb, std::size_t lines) {
    sig::Slot<void()> slot{[&tb, lines] { tb.set_wheel_speed_up(lines); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> set_wheel_speed_up(Textbox& tb) {
    sig::Slot<void(std::size_t)> slot{
        [&tb](auto lines) { tb.set_wheel_speed_up(lines); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> set_wheel_speed_down(Textbox& tb, std::size_t lines) {
    sig::Slot<void()> slot{[&tb, lines] { tb.set_wheel_speed_down(lines); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> set_wheel_speed_down(Textbox& tb) {
    sig::Slot<void(std::size_t)> slot{
        [&tb](auto lines) { tb.set_wheel_speed_down(lines); }};
    slot.track(tb.destroyed);
    return slot;
}

}  // namespace slot

}  // namespace cppurses
