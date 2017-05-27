#include "widget_module/widgets/textbox.hpp"
#include "painter_module/glyph_string.hpp"
#include "system_module/events/key_event.hpp"
#include "system_module/events/mouse_event.hpp"
#include "system_module/key.hpp"
#include "widget_module/coordinate.hpp"
#include "widget_module/focus_policy.hpp"
#include <iterator>
#include <utility>
#include <cstddef>

namespace twf {

Textbox::Textbox(Glyph_string contents) : Textbox_base{std::move(contents)} {
    this->set_focus_policy(Focus_policy::Strong);
}

void Textbox::enable_scroll_wheel(bool enable) {
    scroll_wheel_ = enable;
}

void Textbox::disable_scroll_wheel(bool disable) {
    scroll_wheel_ = !disable;
}

bool Textbox::does_scroll_wheel() const {
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

bool Textbox::key_press_event(const Key_event& event) {
    switch (event.key_code()) {
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
            auto text = event.text();
            if (!text.empty()) {
                auto cursor_index = this->cursor_index();
                this->insert_(text, cursor_index);
                this->cursor_right_(1);
                this->set_cursor_at_index_(cursor_index + 1);
            }
    }
    this->update();
    return true;
}

bool Textbox::mouse_press_event(const Mouse_event& event) {
    if (event.button() == Mouse_button::Left) {
        this->set_cursor_at_coordinates_(event.local_x(), event.local_y());
    } else if (event.button() == Mouse_button::ScrollUp) {
        if (scroll_wheel_) {
            this->scroll_up_(scroll_speed_up_);
        }
    } else if (event.button() == Mouse_button::ScrollDown) {
        if (scroll_wheel_) {
            this->scroll_down_(scroll_speed_down_);
        }
    }
    this->update();
    return true;
}

}  // namespace twf
