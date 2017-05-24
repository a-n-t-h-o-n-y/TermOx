#include <widget_module/widgets/textbox_core.hpp>
#include <painter_module/painter.hpp>
#include <system_module/events/paint_event.hpp>
#include "system_module/events/resize_event.hpp"
#include "widget_module/coordinate.hpp"
#include "painter_module/glyph_string.hpp"

#include <cstddef>
#include <functional>
#include <utility>

namespace twf {

Textbox_core::Textbox_core(Glyph_string string)
    : Text_display{std::move(string)} {
    this->enable_cursor();
};

void Textbox_core::set_cursor_at_coordinates(Coordinate pos) {
    this->set_cursor_at_coordinates(pos.x, pos.y);
}

void Textbox_core::set_cursor_at_coordinates(std::size_t x, std::size_t y) {
    this->set_cursor_at_index(this->index_at(x, y));
}

void Textbox_core::set_cursor_at_index(std::size_t index) {
    this->move_cursor(this->display_position(index));
}

std::size_t Textbox_core::cursor_index() const {
    return this->index_at(this->cursor_coordinate());
}

void Textbox_core::cursor_up(std::size_t n) {
    auto y = this->cursor_y() - n;
    if (this->cursor_y() < n) {
        if (this->does_scroll()) {
            this->scroll_up(n - this->cursor_y());
        }
        y = 0;
    }
    this->set_cursor_at_coordinates(this->cursor_x(), y);
}

void Textbox_core::cursor_down(std::size_t n) {
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
    this->set_cursor_at_coordinates(this->cursor_x(), y);
}

void Textbox_core::cursor_left(std::size_t n) {
    for (std::size_t i{0}; i < n; ++i) {
        this->increment_cursor_left();
    }
}

void Textbox_core::increment_cursor_left() {
    auto next_index = this->cursor_index();
    if (this->cursor_coordinate() == Coordinate{0, 0}) {
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
    this->set_cursor_at_index(next_index);
}

void Textbox_core::cursor_right(std::size_t n) {
    for (std::size_t i{0}; i < n; ++i) {
        this->increment_cursor_right();
    }
}

void Textbox_core::increment_cursor_right() {
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
    this->set_cursor_at_index(cursor_index + 1);
}

void Textbox_core::scroll_up(std::size_t n) {
    if (this->top_line() == 0) {
        return;
    }
    Text_display::scroll_up(n);
    auto y = this->cursor_y() + n;
    if (y > this->height() - 1) {
        y = this->height() - 1;
    }
    this->set_cursor_at_coordinates(this->cursor_x(), y);
}

void Textbox_core::scroll_down(std::size_t n) {
    Text_display::scroll_down(n);
    auto y = this->cursor_y();
    if (y < n) {
        y = 0;
    } else {
        y -= n;
    }
    this->set_cursor_at_coordinates(this->cursor_x(), y);
}

bool Textbox_core::resize_event(const Resize_event& event) {
    const auto cursor_index = this->index_at(this->cursor_coordinate());
    Text_display::resize_event(event);
    // Scroll if old cursor index is now hidden.
    const auto cursor_line = this->line_at(cursor_index);
    if (this->top_line() > cursor_line) {
        this->scroll_up(this->top_line() - cursor_line);
    } else if (this->bottom_line() < cursor_line) {
        this->scroll_down(cursor_line - this->bottom_line());
    }
    this->set_cursor_at_index(cursor_index);
    return Text_display::resize_event(event);
}

}  // namespace twf
