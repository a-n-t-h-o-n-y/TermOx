#include <termox/widget/cursor.hpp>

#include <termox/widget/point.hpp>

namespace ox {

auto Cursor::is_enabled() const -> bool { return enabled_; }

auto Cursor::x() const -> int { return position_.x; }

auto Cursor::y() const -> int { return position_.y; }

auto Cursor::position() const -> Point { return position_; }

void Cursor::enable(bool enable) { enabled_ = enable; }

void Cursor::disable(bool disable) { this->enable(!disable); }

void Cursor::toggle() { this->enable(!this->is_enabled()); }

void Cursor::set_x(int x)
{
    position_.x = x;
    moved.emit(position_);
}

void Cursor::set_y(int y)
{
    position_.y = y;
    moved.emit(position_);
}

void Cursor::set_position(Point p)
{
    this->set_x(p.x);
    this->set_y(p.y);
}

}  // namespace ox
