#include <widget_module/widgets/textbox_core.hpp>
#include <painter_module/painter.hpp>
#include <system_module/events/paint_event.hpp>
#include "system_module/events/resize_event.hpp"
#include <widget_module/coordinate.hpp>

#include <cstddef>
#include <functional>
#include <utility>

namespace twf {

Textbox_core::Textbox_core(Glyph_string string)
    : Text_display{std::move(string)} {
    this->enable_cursor();
};

void Textbox_core::set_cursor_coordinates(Coordinate pos) {
    this->set_cursor_coordinates(pos.x, pos.y);
}

void Textbox_core::set_cursor_coordinates(std::size_t x, std::size_t y) {
    // Off the end of string cursor position.
    auto destination_index = this->index_at(this->top_line() + y) + x;
    if (!this->contents_empty() && destination_index >= this->contents_size()) {
        auto last_pos = this->display_position(this->contents_size() - 1);
        x = ++last_pos.x;
        y = last_pos.y;
        if (x >= this->width()) {
            x = 0;
            ++y;
        }
        // this->move_cursor(x, y);
        // return;
    } else if (y >= this->number_of_rows()) {
        if (this->number_of_rows() == 0) {
            y = 0;
        } else {
            y = this->number_of_rows() - 1;
        }
    } else if (x > this->row_length(y)) {
        // if (this->row_length(y) == 0) {
        //     x = 0;
        // } else {
        x = this->row_length(y);
        // }
    }
    this->move_cursor(x, y);
}

std::size_t Textbox_core::cursor_index() const {
    return this->string_index(this->cursor_coordinate());
}
void Textbox_core::cursor_up(std::size_t n) {
    if (this->cursor_y() == 0 && this->top_line() == 0) {
        return;
    }
    if (this->does_scroll() && this->cursor_y() < n) {
        this->scroll_up(n - this->cursor_y());
        this->set_cursor_coordinates(this->cursor_x(), 0);
        return;
    }
    this->set_cursor_coordinates(this->cursor_x(), this->cursor_y() - n);
}

void Textbox_core::cursor_down(std::size_t n) {
    if (this->does_scroll() && this->height() > 0 &&
        this->cursor_y() + n >= this->height() &&
        this->last_line() >= this->top_line() + this->cursor_y() + n) {
        this->scroll_down(n - (this->height() - 1 - this->cursor_y()));
        this->set_cursor_coordinates(this->cursor_x(), this->height() - 1);
        return;
    }
    this->set_cursor_coordinates(this->cursor_x(), this->cursor_y() + n);
}

void Textbox_core::cursor_left(std::size_t n) {
    for (std::size_t i{0}; i < n; ++i) {
        this->increment_cursor_left();
    }

    // auto x = this->cursor_x();
    // x = (x >= n) ? (x - n) : 0;
    // auto y = this->cursor_y();
    // if (this->cursor_x() == 0) {
    //     if (y != 0) {
    //         --y;
    //     }
    //     // if (this->row_length(y) != 0) {
    //     x = this->row_length(y);  //- n; add this in
    //     // }
    // }
    // this->set_cursor_coordinates(x, y);
}

void Textbox_core::increment_cursor_left() {
    // Special Cases
    // when moving upwards, move x to either length - 1 if width == row_length
    // of the above line, or move x to length otherwise.
    if (this->cursor_x() == 0 && this->cursor_y() == 0) {
        if (this->top_line() == 0) {
            return;
        }
        this->scroll_up(1);
        // duplicated below, shoule be able to combine fingers crossed.
        auto new_x = this->row_length(0);
        if (new_x == this->width()) {
            --new_x;
        }
        this->set_cursor_coordinates(new_x, 0);
        return;
    }
    // Core logic -- TODO: Double check bounds checking - row_length -1
    if (this->cursor_x() == 0) {
        auto new_x = this->row_length(this->cursor_y() - 1);
        if (new_x == this->width()) {
            --new_x;
        }
        this->set_cursor_coordinates(new_x, this->cursor_y() - 1);
    } else {
        this->set_cursor_coordinates(this->cursor_x() - 1, this->cursor_y());
    }
}

void Textbox_core::cursor_right(std::size_t n) {
    for (std::size_t i{0}; i < n; ++i) {
        this->increment_cursor_right();
    }
}

void Textbox_core::increment_cursor_right() {
    // Last Position
    if (this->cursor_y() + this->top_line() == this->last_line() &&
        this->cursor_x() == this->row_length(this->last_line())) {
        return;
    }
    // Core Logic
    auto x = this->cursor_x() + 1;
    auto y = this->cursor_y();
    if (this->cursor_x() == this->row_length(this->cursor_y()) ||
        this->cursor_x() + 1 == this->width()) {
        if (this->does_scroll() && this->cursor_y() + 1 == this->height()) {
            this->scroll_down(1);
        }
        x = 0;
        y = this->cursor_y() + 1;
    }
    this->set_cursor_coordinates(x, y);
}

// void Textbox_core::cursor_right(std::size_t n) {
//     auto cursor_index = this->string_index(this->cursor_coordinate());
//     auto new_index = cursor_index + n;
//     auto line = this->line_at(new_index);
//     auto bottom_line = this->top_line() + this->height();
//     if (bottom_line < line) {
//         this->scroll_down(line - bottom_line);
//     }
//     if (new_index >= this->contents_size()) {
//         auto display = this->display_position(this->contents_size());
//         if (display.x + 1 == this->width()) {
//             this->scroll_down();
//             this->set_cursor_coordinates(0, display.y + 1);
//         } else {
//             this->set_cursor_coordinates(display.x + 1, display.y);
//         }
//     } else {
//         // Skip space at end of line with word wrap
//         if (this->contents().at(cursor_index + 1).str() == "\n")  //&&
//         [> this->display_position(new_index).x != 0)  <] {
//             ++new_index;
//             this->set_cursor_coordinates(this->cursor_x() + 1,
//                                          this->cursor_y());
//             return;
//         }
//         if (this->word_wrap() &&
//             this->display_position(cursor_index).x + 1 == this->width() &&
//             this->contents().at(cursor_index).str() != " ") {
//             // maybe check for newline above too
//             ++new_index;
//         }
//         this->set_cursor_coordinates(this->display_position(new_index));
//     }
// }

// void Textbox_core::scoot_cursor_right() {
//     if (this->cursor_x() + 1 >= this->row_length(this->cursor_y())) {
//         auto cursor_index =
//             this->index_at(this->top_line() + this->cursor_y()) +
//             this->cursor_x();
//         if (cursor_index + 1 == this->contents_size()) {
//             if (this->cursor_x() + 1 == this->width() &&
//                 this->cursor_y() + 1 == this->height()) {
//                 this->scroll_down(1);
//                 this->set_cursor_coordinates(0, this->cursor_y() + 1);
//                 return;
//             }
//             this->set_cursor_coordinates(this->cursor_x() + 1,
//                                          this->cursor_y());
//         } else if (this->cursor_y() + 1 == this->height() &&
//                    this->cursor_x() > 0) {
//             this->scroll_down(1);
//             this->set_cursor_coordinates(0, this->cursor_y() + 1);
//         } else {
//             this->set_cursor_coordinates(0, this->cursor_y() + 1);
//         }
//         return;
//     }
//     this->set_cursor_coordinates(this->cursor_x() + 1, this->cursor_y());
// }

bool Textbox_core::resize_event(const Resize_event& event) {
    auto index = this->string_index(this->cursor_coordinate());
    Text_display::resize_event(event);
    this->set_cursor_coordinates(this->display_position(index));
    return true;
}

void Textbox_core::scroll_up(std::size_t n) {
    if (this->height() == 0 || this->top_line() == 0) {
        return;
    }
    Text_display::scroll_up(n);
    auto x = this->cursor_x();
    auto y = this->cursor_y() + n;
    auto distance_to_bottom = this->height() - 1 - this->cursor_y();
    if (n > distance_to_bottom) {
        auto destination_length = this->row_length(this->cursor_y());
        if (destination_length < x) {
            x = destination_length;
        }
        y = this->height() - 1;
    }
    this->set_cursor_coordinates(x, y);
}

void Textbox_core::scroll_down(std::size_t n) {
    // if (this->cursor_y() != 0) {
    //     auto x = this->cursor_x();
    //     if (this->cursor_x() > this->row_length(this->cursor_y() + 1)) {
    //         x = this->row_length(this->cursor_y() + 1);
    //     }
    //     this->set_cursor_coordinates(x, this->cursor_y() - 1);
    // }
    Text_display::scroll_down(n);
    auto x = this->cursor_x();
    auto y = this->cursor_y();
    if (this->cursor_y() != 0) {
        y -= n;
    }
    if (n > this->cursor_y()) {
        auto new_length = this->row_length(0);
        if (new_length < this->cursor_x()) {
            x = new_length;
        }
    }
    this->set_cursor_coordinates(x, y);
}

// void Textbox_core::scroll_up(std::size_t n) {
//     upper_bound_ = previous_line_break(upper_bound_);
//     lower_bound_ = this->find_lower_bound();
//     if (cursor_index_ > lower_bound_) {
//         this->set_cursor_index(
//             this->index_from_position(0, this->height() - 1));
//     }
//     this->update();
// }

// void Textbox_core::scroll_down(std::size_t n) {
//     for (std::size_t i = upper_bound_; i < contents_.size(); ++i) {
//         if (i - upper_bound_ == this->width() - 1 ||
//             contents_.at(i).str() == "\n") {
//             upper_bound_ = i + 1;
//             break;
//         }
//     }
//     if (cursor_index_ < upper_bound_) {
//         this->set_cursor_index(upper_bound_);
//     }
//     this->update();
// }

// // Moves the cursor up n positions, scrolls if need be.
// void Textbox_core::cursor_up(std::size_t n) {
//     auto pos = this->position_from_index(cursor_index_);
//     if (pos.y == 0) {
//         return;
//     }
//     --pos.y;
//     this->set_cursor_index(this->index_from_position(pos));
// }

// // Moves the cursor down n positions, scrolls if need be.
// void Textbox_core::cursor_down(std::size_t n) {
//     auto pos = this->position_from_index(cursor_index_);
//     if (this->position_from_index(lower_bound_).y == pos.y) {
//         return;
//     }
//     ++pos.y;
//     this->set_cursor_index(this->index_from_position(pos));
// }

// // Moves the cursor left n positions, wraps if need be.
// void Textbox_core::cursor_left(std::size_t n) {
//     if (cursor_index_ != 0) {
//         auto pos = this->position_from_index(cursor_index_);
//         --pos.x;
//         this->set_cursor_index(this->index_from_position(pos));
//     }
// }

// // Moves the cursor right n positions, wraps if need be.
// void Textbox_core::cursor_right(std::size_t n) {
//     if (cursor_index_ != contents_.size()) {
//         auto pos = this->position_from_index(cursor_index_);
//         ++pos.x;
//         this->set_cursor_index(this->index_from_position(pos));
//     }
// }

// // Called by update, defines what it means to paint a widget that is a
// // textbox_core.
// bool Textbox_core::paint_event(const Paint_event& event) {
//     Painter p{this};
//     // Paint the visible sub-string.
//     // auto checked_lb = lower_bound_;
//     // if(lower_bound_ != 0 && lower_bound_ >= contents_.size()) {
//     //     checked_lb = contents_.size() - 1;
//     // }
//     std::size_t upper{0};
//     if (upper_bound_ > contents_.size()) {
//         upper = contents_.size();
//     } else {
//         upper = upper_bound_;
//     }

//     lower_bound_ = find_lower_bound();
//     Glyph_string sub_str(std::begin(contents_) + upper,
//                          std::begin(contents_) + lower_bound_);
//     p.put(sub_str);
//     // Move the cursor to the appropriate position.
//     auto pos = this->position_from_index(cursor_index_);
//     this->move_cursor(pos.x, pos.y);
//     return Widget::paint_event(event);
// }

// bool Textbox_core::resize_event(const Resize_event& event) {
//     Widget::resize_event(event);
//     lower_bound_ = this->find_lower_bound();
//     if (cursor_index_ > lower_bound_) {
//         this->set_cursor_index(
//             this->index_from_position(0, this->height() - 1));
//     }
//     this->update();
//     return true;
// }

// // Finds the location in the Glyph_string where the given position refers
// to.
// std::size_t Textbox_core::index_from_position(std::size_t x, std::size_t
// y) {
//     Coordinate running_position;
//     for (std::size_t i{upper_bound_}; i < contents_.size(); ++i) {
//         // Exact match.
//         if (running_position.x == x && running_position.y == y) {
//             return i;
//         }
//         // Position is past last character on line.
//         if (running_position.y == y && contents_.at(i).str() == "\n") {
//             return i;
//         }
//         // Increment
//         if (contents_.at(i).str() == "\n" ||
//             running_position.x + 1 == this->width()) {
//             ++running_position.y;
//             running_position.x = 0;
//         } else {
//             ++running_position.x;
//         }
//     }
//     return contents_.size();
// }

// std::size_t Textbox_core::index_from_position(Coordinate pos) {
//     return this->index_from_position(pos.x, pos.y);
// }

// // Calculates the position of the cursor on the 2D screen from an index
// that
// // cooresponds to a position within the Glyph_string contents_ member.
// Coordinate Textbox_core::position_from_index(std::size_t index) {
//     Coordinate position;
//     for (std::size_t i{upper_bound_}; i < index; ++i) {
//         if (i == contents_.size()) {
//             return position;
//         }
//         if (contents_.at(i).str() == "\n" || position.x + 1 ==
//         this->width())
//         {
//             ++position.y;
//             position.x = 0;
//         } else {
//             ++position.x;
//         }
//     }
//     return position;
// }

// // Sets the position of the cursor in the Glyph_string. Possibly scrolls
// so
// // that the index position is visible in the window.
// void Textbox_core::set_cursor_index(std::size_t index) {
//     cursor_index_ = index;
//     auto pos = this->position_from_index(cursor_index_);
//     Painter p{this};
//     this->move_cursor(pos.x, pos.y);
// }

// std::size_t Textbox_core::previous_line_break(std::size_t
// current_upper_bound) {
//     if (current_upper_bound == 0) {
//         return 0;
//     }
//     std::size_t previous{0};
//     std::size_t line_index{0};
//     for (std::size_t i{0}; i < current_upper_bound - 1; ++i) {
//         if (contents_.at(i) == '\n') {
//             previous = i + 1;
//             line_index = 0;
//         } else if (line_index + 1 == this->width()) {
//             previous += line_index + 1;
//             line_index = 0;
//         } else {
//             ++line_index;
//         }
//     }
//     return previous;
// }

// std::size_t Textbox_core::find_lower_bound() {
//     std::size_t height{0};
//     std::size_t line_index{0};
//     for (std::size_t i{upper_bound_}; i < contents_.size(); ++i) {
//         ++line_index;
//         if (height == this->height()) {
//             return i;
//         } else if (contents_.at(i) == '\n') {
//             ++height;
//             line_index = 0;
//         } else if (line_index == this->width()) {
//             ++height;
//             line_index = 0;
//         }
//     }
//     return contents_.size();
// }

}  // namespace twf
