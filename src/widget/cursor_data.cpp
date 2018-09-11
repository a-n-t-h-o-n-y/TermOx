#include <cppurses/widget/cursor_data.hpp>

#include <cstddef>

#include <cppurses/widget/widget.hpp>

namespace cppurses {

Cursor_data::Cursor_data(const Widget* owner) : owner_{owner} {}

bool Cursor_data::enabled() const {
    return enabled_;
}

std::size_t Cursor_data::x() const {
    return position_.x;
}

std::size_t Cursor_data::y() const {
    return position_.y;
}

Point Cursor_data::position() const {
    return position_;
}

void Cursor_data::enable(bool enable) {
    enabled_ = enable;
}

void Cursor_data::disable(bool disable) {
    this->enable(!disable);
}

void Cursor_data::toggle() {
    this->enable(!this->enabled());
}

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

void Cursor_data::set_position(Point p) {
    this->set_x(p.x);
    this->set_y(p.y);
}

}  // namespace cppurses
