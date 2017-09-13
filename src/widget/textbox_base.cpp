#include "widget/widgets/textbox_base.hpp"

#include "painter/glyph_string.hpp"
#include "system/events/resize_event.hpp"
#include "widget/coordinates.hpp"
#include "widget/widgets/text_display.hpp"
#include <cstddef>
#include <utility>

namespace cppurses {

Textbox_base::Textbox_base(Glyph_string contents)
    : Text_display{std::move(contents)} {
    this->enable_cursor();
    this->Textbox_base::initialize();
};

void Textbox_base::initialize() {
    cursor_up = [this] { this->cursor_up_(1); };
    cursor_up.track(this->destroyed);
    cursor_down = [this] { this->cursor_down_(1); };
    cursor_down.track(this->destroyed);
    cursor_left = [this] { this->cursor_left_(1); };
    cursor_left.track(this->destroyed);
    cursor_right = [this] { this->cursor_right_(1); };
    cursor_right.track(this->destroyed);
    cursor_up_n = [this](std::size_t n) { this->cursor_up_(n); };
    cursor_up_n.track(this->destroyed);
    cursor_down_n = [this](std::size_t n) { this->cursor_down_(n); };
    cursor_down_n.track(this->destroyed);
    cursor_left_n = [this](std::size_t n) { this->cursor_left_(n); };
    cursor_left_n.track(this->destroyed);
    cursor_right_n = [this](std::size_t n) { this->cursor_right_(n); };
    cursor_right_n.track(this->destroyed);

    set_cursor_at_coordinates = [this](Coordinates c) {
        this->set_cursor_at_coordinates_(c);
    };
    set_cursor_at_coordinates.track(this->destroyed);
    set_cursor_at_coordinates_x_y = [this](std::size_t x, std::size_t y) {
        this->set_cursor_at_coordinates_(x, y);
    };
    set_cursor_at_coordinates_x_y.track(this->destroyed);
    set_cursor_at_index = [this](std::size_t index) {
        this->set_cursor_at_index_(index);
    };
    set_cursor_at_index.track(this->destroyed);

    scroll_up = [this] { this->scroll_up_(1); };
    scroll_up.track(this->destroyed);
    scroll_down = [this] { this->scroll_down_(1); };
    scroll_down.track(this->destroyed);
    scroll_up_n = [this](std::size_t n) { this->scroll_up_(n); };
    scroll_up_n.track(this->destroyed);
    scroll_down_n = [this](std::size_t n) { this->scroll_down_(n); };
    scroll_down_n.track(this->destroyed);

    enable_scrolling = [this] { this->enable_scrolling_(); };
    enable_scrolling.track(this->destroyed);
    disable_scrolling = [this] { this->disable_scrolling_(); };
    disable_scrolling.track(this->destroyed);
    toggle_scrolling = [this] { this->enable_scrolling_(this->does_scroll()); };
    toggle_scrolling.track(this->destroyed);
    set_scrolling = [this](bool enable) { this->enable_scrolling_(enable); };
    set_scrolling.track(this->destroyed);
}

void Textbox_base::set_cursor_at_coordinates_(Coordinates pos) {
    this->set_cursor_at_coordinates_(pos.x, pos.y);
}

void Textbox_base::set_cursor_at_coordinates_(std::size_t x, std::size_t y) {
    this->set_cursor_at_index_(this->index_at(x, y));
}

void Textbox_base::set_cursor_at_index_(std::size_t index) {
    auto coords = this->display_position(index);
    this->move_cursor(coords);
    cursor_moved(coords);
}

std::size_t Textbox_base::cursor_index() const {
    return this->index_at(this->cursor_coordinates());
}

void Textbox_base::cursor_up_(std::size_t n) {
    auto y = this->cursor_y() - n;
    if (this->cursor_y() < n) {
        if (this->does_scroll()) {
            this->scroll_up_(n - this->cursor_y());
        }
        y = 0;
    }
    this->set_cursor_at_coordinates_(this->cursor_x(), y);
    cursor_moved_up(n);
}

void Textbox_base::cursor_down_(std::size_t n) {
    if (this->cursor_y() + this->top_line() == this->last_line()) {
        return;
    }
    auto y = this->cursor_y() + n;
    if (y >= this->height()) {
        if (this->does_scroll()) {
            this->scroll_down_(y - (this->height() - 1));
            y = this->height() - 1;
        } else {
            return;
        }
    }
    this->set_cursor_at_coordinates_(this->cursor_x(), y);
    cursor_moved_down(n);
}

void Textbox_base::cursor_left_(std::size_t n) {
    for (std::size_t i{0}; i < n; ++i) {
        this->increment_cursor_left();
    }
    cursor_moved_left(n);
}

void Textbox_base::increment_cursor_left() {
    auto next_index = this->cursor_index();
    if (this->cursor_coordinates() == Coordinates{0, 0}) {
        if (this->does_scroll()) {
            this->scroll_up_(1);
        } else {
            return;
        }
    }
    if (next_index == 0) {
        return;
    }
    --next_index;
    this->set_cursor_at_index_(next_index);
}

void Textbox_base::cursor_right_(std::size_t n) {
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
            this->scroll_down_(1);
        } else {
            return;
        }
    }
    this->set_cursor_at_index_(cursor_index + 1);
}

void Textbox_base::scroll_up_(std::size_t n) {
    if (this->top_line() == 0) {
        return;
    }
    Text_display::scroll_up_(n);
    auto y = this->cursor_y() + n;
    if (y > this->height() - 1) {
        y = this->height() - 1;
    }
    this->set_cursor_at_coordinates_(this->cursor_x(), y);
}

void Textbox_base::scroll_down_(std::size_t n) {
    Text_display::scroll_down_(n);
    auto y = this->cursor_y();
    if (y < n) {
        y = 0;
    } else {
        y -= n;
    }
    this->set_cursor_at_coordinates_(this->cursor_x(), y);
}

bool Textbox_base::resize_event(const Resize_event& event) {
    const auto cursor_index = this->index_at(this->cursor_coordinates());
    Text_display::resize_event(event);
    // Scroll if old cursor index is now hidden.
    const auto cursor_line = this->line_at(cursor_index);
    if (this->top_line() > cursor_line) {
        this->scroll_up_(this->top_line() - cursor_line);
    } else if (this->bottom_line() < cursor_line) {
        this->scroll_down_(cursor_line - this->bottom_line());
    }
    this->set_cursor_at_index_(cursor_index);
    return Text_display::resize_event(event);
}

}  // namespace cppurses
