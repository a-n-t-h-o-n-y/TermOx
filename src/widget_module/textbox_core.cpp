#include <widget_module/widgets/textbox_core.hpp>
#include <painter_module/painter.hpp>
#include <system_module/events/paint_event.hpp>
#include <widget_module/coordinate.hpp>

#include <cstddef>
#include <functional>

namespace twf {

Textbox_core::Textbox_core(const Glyph_string& string) : contents_{string} {
    this->enable_cursor();
};

void Textbox_core::scroll_up(std::size_t n) {
    upper_bound_ = previous_line_break(upper_bound_);
    lower_bound_ = this->find_lower_bound();
    if (cursor_index_ > lower_bound_) {
        this->set_cursor_index(
            this->index_from_position(0, this->height() - 1));
    }
    this->update();
}

void Textbox_core::scroll_down(std::size_t n) {
    for (std::size_t i = upper_bound_; i < contents_.size(); ++i) {
        if (i - upper_bound_ == this->width() - 1 ||
            contents_.at(i).str() == "\n") {
            upper_bound_ = i + 1;
            break;
        }
    }
    if (cursor_index_ < upper_bound_) {
        this->set_cursor_index(upper_bound_);
    }
    this->update();
}

// Moves the cursor up n positions, scrolls if need be.
void Textbox_core::cursor_up(std::size_t n) {
    auto pos = this->position_from_index(cursor_index_);
    if (pos.y == 0) {
        return;
    }
    --pos.y;
    this->set_cursor_index(this->index_from_position(pos));
}

// Moves the cursor down n positions, scrolls if need be.
void Textbox_core::cursor_down(std::size_t n) {
    auto pos = this->position_from_index(cursor_index_);
    if (this->position_from_index(lower_bound_).y == pos.y) {
        return;
    }
    ++pos.y;
    this->set_cursor_index(this->index_from_position(pos));
}

// Moves the cursor left n positions, wraps if need be.
void Textbox_core::cursor_left(std::size_t n) {
    if (cursor_index_ != 0) {
        auto pos = this->position_from_index(cursor_index_);
        --pos.x;
        this->set_cursor_index(this->index_from_position(pos));
    }
}

// Moves the cursor right n positions, wraps if need be.
void Textbox_core::cursor_right(std::size_t n) {
    if (cursor_index_ != contents_.size()) {
        auto pos = this->position_from_index(cursor_index_);
        ++pos.x;
        this->set_cursor_index(this->index_from_position(pos));
    }
}

// Called by update, defines what it means to paint a widget that is a
// textbox_core.
bool Textbox_core::paint_event(const Paint_event& event) {
    Painter p{this};
    // Paint the visible sub-string.
    // auto checked_lb = lower_bound_;
    // if(lower_bound_ != 0 && lower_bound_ >= contents_.size()) {
    //     checked_lb = contents_.size() - 1;
    // }
    std::size_t upper{0};
    if (upper_bound_ > contents_.size()) {
        upper = contents_.size();
    } else {
        upper = upper_bound_;
    }

    lower_bound_ = find_lower_bound();
    Glyph_string sub_str(std::begin(contents_) + upper,
                         std::begin(contents_) + lower_bound_);
    p.put(sub_str);
    // Move the cursor to the appropriate position.
    auto pos = this->position_from_index(cursor_index_);
    this->move_cursor(pos.x, pos.y);
    return Widget::paint_event(event);
}

bool Textbox_core::resize_event(const Resize_event& event) {
    Widget::resize_event(event);
    lower_bound_ = this->find_lower_bound();
    if (cursor_index_ > lower_bound_) {
        this->set_cursor_index(
            this->index_from_position(0, this->height() - 1));
    }
    this->update();
    return true;
}

// Finds the location in the Glyph_string where the given position refers to.
std::size_t Textbox_core::index_from_position(std::size_t x, std::size_t y) {
    Coordinate running_position;
    for (std::size_t i{upper_bound_}; i < contents_.size(); ++i) {
        // Exact match.
        if (running_position.x == x && running_position.y == y) {
            return i;
        }
        // Position is past last character on line.
        if (running_position.y == y && contents_.at(i).str() == "\n") {
            return i;
        }
        // Increment
        if (contents_.at(i).str() == "\n" ||
            running_position.x + 1 == this->width()) {
            ++running_position.y;
            running_position.x = 0;
        } else {
            ++running_position.x;
        }
    }
    return contents_.size();
}

std::size_t Textbox_core::index_from_position(Coordinate pos) {
    return this->index_from_position(pos.x, pos.y);
}

// Calculates the position of the cursor on the 2D screen from an index that
// cooresponds to a position within the Glyph_string contents_ member.
Coordinate Textbox_core::position_from_index(std::size_t index) {
    Coordinate position;
    for (std::size_t i{upper_bound_}; i < index; ++i) {
        if (i == contents_.size()) {
            return position;
        }
        if (contents_.at(i).str() == "\n" || position.x + 1 == this->width()) {
            ++position.y;
            position.x = 0;
        } else {
            ++position.x;
        }
    }
    return position;
}

// Sets the position of the cursor in the Glyph_string. Possibly scrolls so
// that the index position is visible in the window.
void Textbox_core::set_cursor_index(std::size_t index) {
    cursor_index_ = index;
    auto pos = this->position_from_index(cursor_index_);
    Painter p{this};
    this->move_cursor(pos.x, pos.y);
}

std::size_t Textbox_core::previous_line_break(std::size_t current_upper_bound) {
    if (current_upper_bound == 0) {
        return 0;
    }
    std::size_t previous{0};
    std::size_t line_index{0};
    for (std::size_t i{0}; i < current_upper_bound - 1; ++i) {
        if (contents_.at(i) == '\n') {
            previous = i + 1;
            line_index = 0;
        } else if (line_index + 1 == this->width()) {
            previous += line_index + 1;
            line_index = 0;
        } else {
            ++line_index;
        }
    }
    return previous;
}

std::size_t Textbox_core::find_lower_bound() {
    std::size_t height{0};
    std::size_t line_index{0};
    for (std::size_t i{upper_bound_}; i < contents_.size(); ++i) {
        ++line_index;
        if (height == this->height()) {
            return i;
        } else if (contents_.at(i) == '\n') {
            ++height;
            line_index = 0;
        } else if (line_index == this->width()) {
            ++height;
            line_index = 0;
        }
    }
    return contents_.size();
}

}  // namespace twf
