#include <termox/widget/widgets/detail/textbox_base.hpp>

#include <cstddef>

#include <termox/painter/glyph_string.hpp>
#include <termox/widget/widgets/text_display.hpp>

namespace ox::detail {

void Textbox_base::cursor_up(std::size_t n)
{
    auto y = this->cursor.y() - n;
    if (this->cursor.y() < n) {
        this->scroll_up(n - this->cursor.y());
        y = 0;
    }
    this->set_cursor({this->cursor.x(), y});
    cursor_moved_up(n);
}

void Textbox_base::cursor_down(std::size_t n)
{
    if (this->cursor.y() + this->top_line() == this->last_line())
        return;
    auto y = this->cursor.y() + n;
    if (y >= this->height()) {
        this->scroll_down(y - (this->height() - 1));
        y = this->height() - 1;
    }
    this->set_cursor({this->cursor.x(), y});
    cursor_moved_down(n);
}

auto Textbox_base::resize_event(Area new_size, Area old_size) -> bool
{
    auto const cursor_index = this->index_at(this->cursor.position());
    Text_display::resize_event(new_size, old_size);

    // Scroll if old cursor index is now hidden.
    auto const cursor_line = this->line_at(cursor_index);
    if (this->top_line() > cursor_line)
        this->scroll_up(this->top_line() - cursor_line);
    else if (this->bottom_line() < cursor_line)
        this->scroll_down(cursor_line - this->bottom_line());
    this->set_cursor(cursor_index);
    return Text_display::resize_event(new_size, old_size);
}

}  // namespace ox::detail
