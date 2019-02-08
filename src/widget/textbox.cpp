#include <cppurses/widget/widgets/textbox.hpp>

#include <cstddef>
#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/focus_policy.hpp>

namespace cppurses {

bool Textbox::key_press_event(const Keyboard_data& keyboard) {
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

    if (takes_input_) {
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
    }
    // this->update();
    return true;
}

bool Textbox::mouse_press_event(const Mouse_data& mouse) {
    if (mouse.button == Mouse_button::Left) {
        this->set_cursor({mouse.local.x, mouse.local.y});
    } else if (mouse.button == Mouse_button::ScrollUp) {
        if (scroll_wheel_) {
            this->scroll_up(scroll_speed_up_);
        }
    } else if (mouse.button == Mouse_button::ScrollDown) {
        if (scroll_wheel_) {
            this->scroll_down(scroll_speed_down_);
        }
    }
    this->update();
    return Widget::mouse_press_event(mouse);
}

}  // namespace cppurses
