#include <termox/widget/cursor.hpp>

#include <termox/widget/point.hpp>

namespace ox {

auto Cursor::position() const -> Point { return position_; }

auto Cursor::is_enabled() const -> bool { return enabled_; }

void Cursor::enable(bool enable) { enabled_ = enable; }

void Cursor::disable(bool disable) { this->enable(!disable); }

void Cursor::toggle() { this->enable(!this->is_enabled()); }

void Cursor::set_position(Point p)
{
    position_ = p;
    moved.emit(position_);
}

}  // namespace ox
