#include <cppurses/widget/widgets/detail/textbox_base.hpp>

#include <cstddef>
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace cppurses {
namespace detail {

void Textbox_base::set_cursor(std::size_t index) {
    auto coords = this->display_position(index);
    this->cursor.set_position(coords);
}

void Textbox_base::cursor_up(std::size_t n) {
    auto y = this->cursor.y() - n;
    if (this->cursor.y() < n) {
        this->scroll_up(n - this->cursor.y());
        y = 0;
    }
    this->set_cursor({this->cursor.x(), y});
    cursor_moved_up(n);
}

void Textbox_base::cursor_down(std::size_t n) {
    if (this->cursor.y() + this->top_line() == this->last_line()) {
        return;
    }
    auto y = this->cursor.y() + n;
    if (y >= this->height()) {
        this->scroll_down(y - (this->height() - 1));
        y = this->height() - 1;
    }
    this->set_cursor({this->cursor.x(), y});
    cursor_moved_down(n);
}

void Textbox_base::cursor_left(std::size_t n) {
    for (std::size_t i{0}; i < n; ++i) {
        this->increment_cursor_left();
    }
    cursor_moved_left(n);
}

void Textbox_base::cursor_right(std::size_t n) {
    for (std::size_t i{0}; i < n; ++i) {
        this->increment_cursor_right();
    }
    cursor_moved_right(n);
}

Textbox_base::Textbox_base(Glyph_string contents)
    : Text_display{std::move(contents)} {
    this->cursor.enable();
}

void Textbox_base::scroll_up(std::size_t n) {
    if (this->top_line() == 0) {
        return;
    }
    Text_display::scroll_up(n);
    auto y = this->cursor.y() + n;
    if (y > this->height() - 1) {
        y = this->height() - 1;
    }
    this->set_cursor({this->cursor.x(), y});
}

void Textbox_base::scroll_down(std::size_t n) {
    Text_display::scroll_down(n);
    auto y = this->cursor.y();
    if (y < n) {
        y = 0;
    } else {
        y -= n;
    }
    this->set_cursor({this->cursor.x(), y});
}

bool Textbox_base::resize_event(Area new_size, Area old_size) {
    const auto cursor_index = this->index_at(this->cursor.position());
    Text_display::resize_event(new_size, old_size);
    // Scroll if old cursor index is now hidden.
    const auto cursor_line = this->line_at(cursor_index);
    if (this->top_line() > cursor_line) {
        this->scroll_up(this->top_line() - cursor_line);
    } else if (this->bottom_line() < cursor_line) {
        this->scroll_down(cursor_line - this->bottom_line());
    }
    this->set_cursor(cursor_index);
    return Text_display::resize_event(new_size, old_size);
}

void Textbox_base::increment_cursor_left() {
    auto next_index = this->cursor_index();
    if (this->cursor.position() == Point{0, 0}) {
        this->scroll_up(1);
    }
    if (next_index == 0) {
        return;
    }
    --next_index;
    this->set_cursor(next_index);
}

void Textbox_base::increment_cursor_right() {
    if (this->cursor_index() == this->contents().size()) {
        return;
    }
    auto true_last_index = this->first_index_at(this->bottom_line() + 1) - 1;
    auto cursor_index = this->cursor_index();
    if (cursor_index == true_last_index &&
        this->cursor.y() == this->height() - 1) {
        this->scroll_down(1);
    }
    this->set_cursor(cursor_index + 1);
}

}  // namespace detail
}  // namespace cppurses
