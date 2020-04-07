#include <cppurses/widget/widgets/textbox.hpp>

#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>

namespace cppurses {

auto Textbox::key_press_event(Key::State const& keyboard) -> bool
{
    switch (keyboard.key) {
        case Key::Arrow_right: this->cursor_right(1); break;
        case Key::Arrow_left: this->cursor_left(1); break;
        case Key::Arrow_up: this->cursor_up(1); break;
        case Key::Arrow_down: this->cursor_down(1); break;
        default: break;
    }
    if (!takes_input_)
        return Textbox_base::key_press_event(keyboard);
    switch (keyboard.key) {
        case Key::Backspace:
        case Key::Backspace_2: {
            auto cursor_index = this->cursor_index();
            if (cursor_index == 0)
                break;
            this->erase(--cursor_index, 1);
            if (this->line_at(cursor_index) < this->top_line())
                this->scroll_up(1);
            this->set_cursor(cursor_index);
        } break;

        case Key::Enter: {
            auto const cursor_index = this->cursor_index();
            this->insert('\n', cursor_index);
            if (this->cursor.y() + 1 == this->height())
                this->scroll_down(1);
            this->set_cursor(cursor_index + 1);
        } break;

        case Key::Tab:
            // Insert '\t', it will be taken care of in update_display()
            break;

        default:  // Insert text
            auto const text = keyboard.symbol;
            if (text != '\0') {
                // TODO Cursor Movement for Alignments other than left
                auto const cursor_index = this->cursor_index();
                this->insert(text, cursor_index);
                this->cursor_right(1);
                this->set_cursor(cursor_index + 1);
            }
    }
    return Textbox_base::key_press_event(keyboard);
    ;
}

auto Textbox::mouse_press_event(Mouse::State const& mouse) -> bool
{
    if (mouse.button == Mouse::Button::Left)
        this->set_cursor({mouse.local.x, mouse.local.y});
    return Textbox_base::mouse_press_event(mouse);
}

auto Textbox::mouse_wheel_event(Mouse::State const& mouse) -> bool
{
    switch (mouse.button) {
        case Mouse::Button::ScrollUp:
            if (scroll_wheel_)
                this->scroll_up(scroll_speed_up_);
            break;
        case Mouse::Button::ScrollDown:
            if (scroll_wheel_)
                this->scroll_down(scroll_speed_down_);
            break;
        default: break;
    }
    return Textbox_base::mouse_wheel_event(mouse);
}

}  // namespace cppurses
