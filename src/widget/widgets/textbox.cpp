#include <termox/widget/widgets/textbox.hpp>

#include <memory>
#include <utility>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/widgets/detail/textbox_base.hpp>
namespace ox {

Textbox::Textbox(Glyph_string text,
                 Align alignment,
                 Wrap wrap,
                 Brush insert_brush,
                 int scroll_speed,
                 bool text_input)
    : Textbox_base{std::move(text), alignment, wrap, insert_brush},
      scroll_speed_{scroll_speed},
      takes_input_{text_input}
{
    this->focus_policy = Focus_policy::Strong;
}

Textbox::Textbox(Parameters p)
    : Textbox{std::move(p.text),         p.alignment,    p.wrap,
              std::move(p.insert_brush), p.scroll_speed, p.text_input}
{}

void Textbox::set_scroll_speed(int x) noexcept { scroll_speed_ = x; }

auto Textbox::scroll_speed() const noexcept -> int { return scroll_speed_; }

void Textbox::enable_text_input() noexcept { takes_input_ = true; }

void Textbox::disable_text_input() noexcept { takes_input_ = false; }

auto Textbox::has_text_input() const noexcept -> bool { return takes_input_; }

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
        case Key::Backspace_1:
        case Key::Backspace_2: {
            auto cursor_index = this->cursor_index();
            if (cursor_index == 0)
                break;
            this->erase(--cursor_index, 1);
            if (this->line_at(cursor_index) < this->top_line())
                this->scroll_up(1);
            this->set_cursor(cursor_index);
        } break;

        case Key::Delete: {
            auto cursor_index = this->cursor_index();
            if (cursor_index == this->text().size())
                break;
            this->erase(cursor_index, 1);
            if (this->line_at(cursor_index) < this->top_line())
                this->scroll_up(1);
            this->set_cursor(cursor_index);
        } break;

        case Key::Enter: {
            auto const cursor_index = this->cursor_index();
            this->Text_view::insert(U"\n", cursor_index);
            if (this->cursor.position().y + 1 == this->area().height)
                this->scroll_down(1);
            this->set_cursor(cursor_index + 1);
        } break;

        case Key::Tab:
            // Insert '\t', it will be taken care of in update_display()
            break;

        default:  // Insert text
            auto const text = key_to_char32(k);
            if (text != U'\0') {
                // TODO Cursor Movement for Alignments other than left
                auto const cursor_index = this->cursor_index();
                this->Text_view::insert(Glyph_string{text}, cursor_index);
                this->cursor_right(1);
                this->set_cursor(cursor_index + 1);
            }
    }
    return Textbox_base::key_press_event(k);
}

auto Textbox::mouse_press_event(Mouse const& m) -> bool
{
    if (m.button == Mouse::Button::Left)
        this->set_cursor(m.at);
    return Textbox_base::mouse_press_event(m);
}

auto Textbox::mouse_wheel_event(Mouse const& m) -> bool
{
    switch (m.button) {
        case Mouse::Button::ScrollUp: this->scroll_up(scroll_speed_); break;
        case Mouse::Button::ScrollDown: this->scroll_down(scroll_speed_); break;
        default: break;
    }
    return Textbox_base::mouse_wheel_event(m);
}

auto textbox(Glyph_string text,
             Align alignment,
             Wrap wrap,
             Brush insert_brush,
             int scroll_speed,
             bool text_input) -> std::unique_ptr<Textbox>
{
    return std::make_unique<Textbox>(std::move(text), alignment, wrap,
                                     std::move(insert_brush), scroll_speed,
                                     text_input);
}

auto textbox(Textbox::Parameters p) -> std::unique_ptr<Textbox>
{
    return std::make_unique<Textbox>(std::move(p));
}

}  // namespace ox
