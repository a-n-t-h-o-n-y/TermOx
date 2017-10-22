#include <cppurses/widget/widgets/textbox_base.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/coordinates.hpp>
#include <cppurses/widget/widgets/text_display.hpp>
#include <cppurses/widget/widget.hpp>

#include <signals/signal.hpp>

#include <cstddef>
#include <utility>

namespace cppurses {

Textbox_base::Textbox_base(Glyph_string contents)
    : Text_display{std::move(contents)} {
    this->show_cursor();
};

void Textbox_base::set_cursor(Coordinates pos) {
    this->set_cursor(pos.x, pos.y);
}

void Textbox_base::set_cursor(std::size_t x, std::size_t y) {
    this->set_cursor(this->index_at(x, y));
}

void Textbox_base::set_cursor(std::size_t index) {
    auto coords = this->display_position(index);
    move_cursor(*this, coords);
    cursor_moved(coords);
}

std::size_t Textbox_base::cursor_index() const {
    return this->index_at(this->cursor_coordinates());
}

void Textbox_base::cursor_up(std::size_t n) {
    auto y = this->cursor_y() - n;
    if (this->cursor_y() < n) {
        if (this->does_scroll()) {
            this->scroll_up(n - this->cursor_y());
        }
        y = 0;
    }
    this->set_cursor(this->cursor_x(), y);
    cursor_moved_up(n);
}

void Textbox_base::cursor_down(std::size_t n) {
    if (this->cursor_y() + this->top_line() == this->last_line()) {
        return;
    }
    auto y = this->cursor_y() + n;
    if (y >= this->height()) {
        if (this->does_scroll()) {
            this->scroll_down(y - (this->height() - 1));
            y = this->height() - 1;
        } else {
            return;
        }
    }
    this->set_cursor(this->cursor_x(), y);
    cursor_moved_down(n);
}

void Textbox_base::cursor_left(std::size_t n) {
    for (std::size_t i{0}; i < n; ++i) {
        this->increment_cursor_left();
    }
    cursor_moved_left(n);
}

void Textbox_base::increment_cursor_left() {
    auto next_index = this->cursor_index();
    if (this->cursor_coordinates() == Coordinates{0, 0}) {
        if (this->does_scroll()) {
            this->scroll_up(1);
        } else {
            return;
        }
    }
    if (next_index == 0) {
        return;
    }
    --next_index;
    this->set_cursor(next_index);
}

void Textbox_base::cursor_right(std::size_t n) {
    for (std::size_t i{0}; i < n; ++i) {
        this->increment_cursor_right();
    }
    cursor_moved_right(n);
}

void Textbox_base::increment_cursor_right() {
    if (this->cursor_index() == this->contents_size()) {
        return;
    }
    auto true_last_index = this->first_index_at(this->bottom_line() + 1) - 1;
    auto cursor_index = this->cursor_index();
    if (cursor_index == true_last_index &&
        this->cursor_y() == this->height() - 1) {
        if (this->does_scroll()) {
            this->scroll_down(1);
        } else {
            return;
        }
    }
    this->set_cursor(cursor_index + 1);
}

void Textbox_base::scroll_up(std::size_t n) {
    if (this->top_line() == 0) {
        return;
    }
    Text_display::scroll_up(n);
    auto y = this->cursor_y() + n;
    if (y > this->height() - 1) {
        y = this->height() - 1;
    }
    this->set_cursor(this->cursor_x(), y);
}

void Textbox_base::scroll_down(std::size_t n) {
    Text_display::scroll_down(n);
    auto y = this->cursor_y();
    if (y < n) {
        y = 0;
    } else {
        y -= n;
    }
    this->set_cursor(this->cursor_x(), y);
}

void Textbox_base::enable_scrolling(bool enable) {
    scroll_ = enable;
}

void Textbox_base::disable_scrolling(bool disable) {
    scroll_ = !disable;
}

void Textbox_base::toggle_scrolling() {
    scroll_ = !scroll_;
}

bool Textbox_base::resize_event(std::size_t new_width,
                                std::size_t new_height,
                                std::size_t old_width,
                                std::size_t old_height) {
    const auto cursor_index = this->index_at(this->cursor_coordinates());
    Text_display::resize_event(new_width, new_height, old_width, old_height);
    // Scroll if old cursor index is now hidden.
    const auto cursor_line = this->line_at(cursor_index);
    if (this->top_line() > cursor_line) {
        this->scroll_up(this->top_line() - cursor_line);
    } else if (this->bottom_line() < cursor_line) {
        this->scroll_down(cursor_line - this->bottom_line());
    }
    this->set_cursor(cursor_index);
    return Text_display::resize_event(new_width, new_height, old_width,
                                      old_height);
}

}  // namespace cppurses
