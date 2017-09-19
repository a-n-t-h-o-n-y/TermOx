#include "widget/widgets/textbox.hpp"
#include "painter/glyph_string.hpp"
#include "system/events/key_event.hpp"
#include "system/events/mouse_event.hpp"
#include "system/key.hpp"
#include "widget/coordinates.hpp"
#include "widget/focus_policy.hpp"
#include <iterator>
#include <utility>
#include <cstddef>

namespace cppurses {

Textbox::Textbox(Glyph_string contents) : Textbox_base{std::move(contents)} {
    this->set_focus_policy(Focus_policy::Strong);
}

void Textbox::enable_scrollwheel(bool enable) {
    scroll_wheel_ = enable;
}

void Textbox::disable_scrollwheel(bool disable) {
    scroll_wheel_ = !disable;
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
            this->erase_(--cursor_index, 1);
            if (this->line_at(cursor_index) < this->top_line()) {
                this->scroll_up_(1);
            }
            this->set_cursor_at_index_(cursor_index);
        } break;

        case Key::Enter: {
            auto cursor_index = this->cursor_index();
            this->insert_('\n', cursor_index);
            if (this->cursor_y() + 1 == this->height()) {
                this->scroll_down_(1);
            }
            this->set_cursor_at_index_(cursor_index + 1);
        } break;

        case Key::Tab:
            // Insert '\t', it will be taken care of in update_display()
            break;

        case Key::Arrow_right:
            this->cursor_right_(1);
            break;

        case Key::Arrow_left:
            this->cursor_left_(1);
            break;

        case Key::Arrow_up:
            this->cursor_up_(1);
            break;

        case Key::Arrow_down:
            this->cursor_down_(1);
            break;

        default:  // Insert text
            char text = symbol;
            if (text != '\0') {
                auto cursor_index = this->cursor_index();
                this->insert_(text, cursor_index);
                this->cursor_right_(1);
                this->set_cursor_at_index_(cursor_index + 1);
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
        this->set_cursor_at_coordinates_(local_x, local_y);
    } else if (button == Mouse_button::ScrollUp) {
        if (scroll_wheel_) {
            this->scroll_up_(scroll_speed_up_);
        }
    } else if (button == Mouse_button::ScrollDown) {
        if (scroll_wheel_) {
            this->scroll_down_(scroll_speed_down_);
        }
    }
    this->update();
    return true;
}

}  // namespace cppurses
