#include <termox/widget/widgets/detail/textbox_base.hpp>

#include <algorithm>
#include <cstddef>
#include <utility>

namespace ox::detail {

Textbox_base::Textbox_base(Glyph_string text,
                           Align alignment,
                           Wrap wrap,
                           Brush insert_brush_)
    : Text_view{std::move(text), alignment, wrap, std::move(insert_brush_)}
{
    this->cursor.enable();
}

Textbox_base::Textbox_base(Parameters p) : Text_view{std::move(p)} {}

void Textbox_base::set_cursor(Point position)
{
    this->set_cursor(this->index_at(position));
}

void Textbox_base::set_cursor(std::size_t index)
{
    auto const coords = this->display_position(index);
    this->cursor.set_position(coords);
}

void Textbox_base::scroll_up(int n)
{
    if (this->top_line() == 0)
        return;
    Text_view::scroll_up(n);
    auto const y = std::min(this->cursor.position().y + n,
                            std::max(this->area().height - 1, 0));
    this->set_cursor({this->cursor.position().x, y});
}

void Textbox_base::scroll_down(int n)
{
    Text_view::scroll_down(n);
    auto const y = std::max(this->cursor.position().y - n, 0);
    this->set_cursor({this->cursor.position().x, y});
}

void Textbox_base::cursor_up(int n)
{
    auto y = this->cursor.position().y - n;
    if (this->cursor.position().y < n) {
        this->scroll_up(n - this->cursor.position().y);
        y = 0;
    }
    this->set_cursor({this->cursor.position().x, y});
    cursor_moved_up(n);
}

void Textbox_base::cursor_down(int n)
{
    if (this->cursor.position().y + this->top_line() == this->last_line())
        return;
    auto y       = this->cursor.position().y + n;
    auto const h = this->area().height;
    if (y >= h) {
        this->scroll_down(y - (h - 1));
        y = h - 1;
    }
    this->set_cursor({this->cursor.position().x, y});
    cursor_moved_down(n);
}

void Textbox_base::cursor_left(int n)
{
    for (auto i = 0; i < n; ++i)
        this->increment_cursor_left();
    cursor_moved_left(n);
}

void Textbox_base::cursor_right(int n)
{
    for (auto i = 0; i < n; ++i)
        this->increment_cursor_right();
    cursor_moved_right(n);
}

auto Textbox_base::cursor_index() const -> int
{
    return this->index_at(this->cursor.position());
}

auto Textbox_base::resize_event(Area new_size, Area old_size) -> bool
{
    auto const cursor_index = this->index_at(this->cursor.position());
    Text_view::resize_event(new_size, old_size);

    // Scroll if old cursor index is now hidden.
    auto const cursor_line = this->line_at(cursor_index);
    if (this->top_line() > cursor_line)
        this->scroll_up(this->top_line() - cursor_line);
    else if (this->bottom_line() < cursor_line)
        this->scroll_down(cursor_line - this->bottom_line());
    this->set_cursor(cursor_index);
    return Text_view::resize_event(new_size, old_size);
}

void Textbox_base::increment_cursor_right()
{
    if (this->cursor_index() == this->text().size())
        return;
    auto const true_last_index =
        this->first_index_at(this->bottom_line() + 1) - 1;
    auto const cursor_index = this->cursor_index();
    if (cursor_index == true_last_index &&
        this->cursor.position().y == this->area().height - 1) {
        this->cursor.set_position({0, 0});  // Hack for Typer TODO remove?
        this->scroll_down(1);
    }
    this->set_cursor(cursor_index + 1);
}

void Textbox_base::increment_cursor_left()
{
    auto next_index = this->cursor_index();
    if (this->cursor.position() == Point{0, 0})
        this->scroll_up(1);
    if (next_index == 0)
        return;
    --next_index;
    this->set_cursor(next_index);
}

}  // namespace ox::detail
