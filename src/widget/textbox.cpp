#include <cppurses/widget/widgets/textbox.hpp>

#include <cstddef>
#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/widgets/detail/textbox_base.hpp>

namespace cppurses {

Textbox::Textbox(Glyph_string contents) : Textbox_base{std::move(contents)} {
    this->set_name("Textbox");
    this->focus_policy = Focus_policy::Strong;
}

void Textbox::set_wheel_speed(std::size_t lines) {
    this->set_wheel_speed_up(lines);
    this->set_wheel_speed_down(lines);
}

bool Textbox::key_press_event(const Key::State& keyboard) {
    switch (keyboard.key) {
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
        default:
            break;
    }
    if (!takes_input_) {
        return true;
    }
    switch (keyboard.key) {
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
            if (this->cursor.y() + 1 == this->height()) {
                this->scroll_down(1);
            }
            this->set_cursor(cursor_index + 1);
        } break;

        case Key::Tab:
            // Insert '\t', it will be taken care of in update_display()
            break;

        default:  // Insert text
            char text = keyboard.symbol;
            if (text != '\0') {
                // TODO Cursor Movement for Alignments other than left
                auto cursor_index = this->cursor_index();
                this->insert(text, cursor_index);
                this->cursor_right(1);
                this->set_cursor(cursor_index + 1);
            }
    }
    return true;
}

bool Textbox::mouse_press_event(const Mouse::State& mouse) {
    if (mouse.button == Mouse::Button::Left) {
        this->set_cursor({mouse.local.x, mouse.local.y});
    } else if (mouse.button == Mouse::Button::ScrollUp) {
        if (scroll_wheel_) {
            this->scroll_up(scroll_speed_up_);
        }
    } else if (mouse.button == Mouse::Button::ScrollDown) {
        if (scroll_wheel_) {
            this->scroll_down(scroll_speed_down_);
        }
    }
    this->update();
    return Widget::mouse_press_event(mouse);
}

}  // namespace cppurses
