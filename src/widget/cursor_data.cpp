#include <cppurses/widget/cursor_data.hpp>

#include <cstddef>

#include <cppurses/widget/widget.hpp>

namespace cppurses {

void Cursor_data::set_x(std::size_t x) {
    if (x < owner_->width()) {
        position_.x = x;
    } else if (owner_->width() != 0) {
        position_.x = owner_->width() - 1;
    }
    this->moved(position_);
}

void Cursor_data::set_y(std::size_t y) {
    if (y < owner_->height()) {
        position_.y = y;
    } else if (owner_->height() != 0) {
        position_.y = owner_->height() - 1;
    }
    this->moved(position_);
}

}  // namespace cppurses
