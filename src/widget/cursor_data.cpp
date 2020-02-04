#include <cppurses/widget/cursor_data.hpp>

#include <cstddef>

#include <cppurses/widget/widget.hpp>

namespace cppurses {

void Cursor_data::set_x(std::size_t x)
{
    auto const width = owner_->width();
    if (x < width)
        position_.x = x;
    else if (width != 0)
        position_.x = width - 1;
    this->moved(position_);
}

void Cursor_data::set_y(std::size_t y)
{
    auto const height = owner_->height();
    if (y < height)
        position_.y = y;
    else if (height != 0)
        position_.y = owner_->height() - 1;
    this->moved(position_);
}

}  // namespace cppurses
