#include "paint_area.hpp"

#include <cppurses/cppurses.hpp>

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <utility>

#include <signals/slot.hpp>

using namespace cppurses;

Paint_area::Paint_area() {
    enable_border(*this);
    this->width_policy.stretch(5);
    this->show_cursor();
    this->focus_policy = Focus_policy::Strong;
}

void Paint_area::set_symbol(Glyph symbol) {
    current_symbol_ = std::move(symbol);
}

bool Paint_area::mouse_press_event(Mouse_button button,
                                   std::size_t global_x,
                                   std::size_t global_y,
                                   std::size_t local_x,
                                   std::size_t local_y,
                                   std::uint8_t device_id) {
    matrix(local_x, local_y) = current_symbol_;
    this->update();
    return Matrix_display::mouse_press_event(button, global_x, global_y,
                                             local_x, local_y, device_id);
}

bool Paint_area::resize_event(std::size_t new_width,
                              std::size_t new_height,
                              std::size_t old_width,
                              std::size_t old_height) {
    // Only expand the matrix, do not delete it
    if (new_width > matrix.width() || new_height > matrix.height()) {
        matrix.resize(new_width, new_height);
    }
    return Matrix_display::resize_event(new_width, new_height, old_width,
                                        old_height);
}

bool Paint_area::key_press_event(Key key, char symbol) {
    if (this->width() == 0 || this->height() == 0) {
        return Matrix_display::key_press_event(key, symbol);
    }
    std::size_t new_x{this->cursor_x() + 1};
    std::size_t new_y{this->cursor_y() + 1};
    switch (key) {
        case Key::Arrow_right:
            if (new_x == this->width()) {
                new_x = 0;
            }
            this->move_cursor_x(new_x);
            break;
        case Key::Arrow_left:
            this->move_cursor_x(this->cursor_x() - 1);
            break;
        case Key::Arrow_down:
            if (new_y == this->height()) {
                new_y = 0;
            }
            this->move_cursor_y(new_y);
            break;
        case Key::Arrow_up:
            this->move_cursor_y(this->cursor_y() - 1);
            break;
        case Key::Enter:
            matrix.at(this->cursor_x(), this->cursor_y()) = current_symbol_;
            this->update();
            break;
        default:
            if (!std::iscntrl(symbol)) {
                current_symbol_.set_symbol(symbol);
                matrix.at(this->cursor_x(), this->cursor_y()) = current_symbol_;
                this->update();
            }
            break;
    }
    return Matrix_display::key_press_event(key, symbol);
}

namespace slot {

sig::Slot<void(Glyph)> set_symbol(Paint_area& pa) {
    sig::Slot<void(Glyph)> slot{
        [&pa](Glyph symbol) { pa.set_symbol(std::move(symbol)); }};
    slot.track(pa.destroyed);
    return slot;
}

sig::Slot<void()> set_symbol(Paint_area& pa, const Glyph& symbol) {
    sig::Slot<void()> slot{[&pa, symbol] { pa.set_symbol(symbol); }};
    slot.track(pa.destroyed);
    return slot;
}

}  // namespace slot
