#include <cppurses/widget/widgets/textbox.hpp>

#include <cstddef>
#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/widget/focus_policy.hpp>

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

void Textbox::disable_input() {
    takes_input_ = false;
}

void Textbox::enable_input() {
    takes_input_ = true;
}

bool Textbox::key_press_event(Key key, char symbol) {
    switch (key) {
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
    }

    if (takes_input_) {
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
                if (this->cursor.y() + 1 == this->height()) {
                    this->scroll_down(1);
                }
                this->set_cursor(cursor_index + 1);
            } break;

            case Key::Tab:
                // Insert '\t', it will be taken care of in update_display()
                break;

            default:  // Insert text
                char text = symbol;
                if (text != '\0') {
                    // TODO Cursor Movement for Alignments other than left
                    auto cursor_index = this->cursor_index();
                    this->insert(text, cursor_index);
                    this->cursor_right(1);
                    this->set_cursor(cursor_index + 1);
                }
        }
    }
    // TODO maybe dont need because system does not necessarily change, update
    // where the state of the cursor or internal text changes.
    this->update();
    return true;
}

bool Textbox::mouse_press_event(Mouse_button button,
                                Point global,
                                Point local,
                                std::uint8_t device_id) {
    if (button == Mouse_button::Left) {
        this->set_cursor(local.x, local.y);
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

}  // namespace cppurses
