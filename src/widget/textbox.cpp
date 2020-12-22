#include <termox/widget/widgets/textbox.hpp>

#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>

namespace ox {

auto Textbox::key_press_event(Key k) -> bool
{
    switch (k) {
        case Key::Arrow_right: this->cursor_right(1); break;
        case Key::Arrow_left: this->cursor_left(1); break;
        case Key::Arrow_up: this->cursor_up(1); break;
        case Key::Arrow_down: this->cursor_down(1); break;
        default: break;
    }
    if (!takes_input_)
        return Textbox_base::key_press_event(k);
    switch (k) {
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
            this->Text_display::insert(L"\n", cursor_index);
            if (this->cursor.y() + 1 == this->height())
                this->scroll_down(1);
            this->set_cursor(cursor_index + 1);
        } break;

        case Key::Tab:
            // Insert '\t', it will be taken care of in update_display()
            break;

        default:  // Insert text
            auto const text = to_wchar(k);
            if (text != L'\0') {
                // TODO Cursor Movement for Alignments other than left
                auto const cursor_index = this->cursor_index();
                this->Text_display::insert(Glyph_string{text}, cursor_index);
                this->cursor_right(1);
                this->set_cursor(cursor_index + 1);
            }
    }
    return Textbox_base::key_press_event(k);
}

auto Textbox::mouse_press_event(Mouse const& m) -> bool
{
    if (m.button == Mouse::Button::Left)
        this->set_cursor(m.local);
    return Textbox_base::mouse_press_event(m);
}

auto Textbox::mouse_wheel_event(Mouse const& m) -> bool
{
    switch (m.button) {
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
    return Textbox_base::mouse_wheel_event(m);
}

}  // namespace ox
