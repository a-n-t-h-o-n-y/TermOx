#ifndef TERMOX_WIDGET_CURSOR_DATA_HPP
#define TERMOX_WIDGET_CURSOR_DATA_HPP
#include <signals_light/signal.hpp>

#include <termox/widget/point.hpp>

namespace ox {
class Widget;

/// Holds and provides access to all data relevant to a Widget's cursor.
class Cursor {
   public:
    /// Signal called when the cursor is moved, passing along the new position.
    sl::Signal<void(Point)> moved;

   public:
    /// Query if the cursor is enabled.
    [[nodiscard]] auto is_enabled() const -> bool { return enabled_; }

    /// Get the local x coordinate of the cursor.
    [[nodiscard]] auto x() const -> int { return position_.x; }

    /// Get the local y coordinate of the cursor.
    [[nodiscard]] auto y() const -> int { return position_.y; }

    /// Get the local position of the cursor.
    [[nodiscard]] auto position() const -> Point { return position_; }

    /// Enable the cursor(show on screen).
    void enable(bool enable = true) { enabled_ = enable; }

    /// Disable the cursor(do not show on screen).
    void disable(bool disable = true) { this->enable(!disable); }

    /// Enable the cursor if disabled, or disable it if enabled.
    void toggle() { this->enable(!this->is_enabled()); }

    /// Set the local x coordinate of the cursor.
    void set_x(int x)
    {
        position_.x = x;
        this->moved(position_);
    }

    /// Set the local y coordinate of the cursor.
    void set_y(int y)
    {
        position_.y = y;
        this->moved(position_);
    }

    /// Set the local position of the cursor.
    void set_position(Point p)
    {
        this->set_x(p.x);
        this->set_y(p.y);
    }

   private:
    Point position_ = {0, 0};
    bool enabled_   = false;
};

}  // namespace ox
#endif  // TERMOX_WIDGET_CURSOR_DATA_HPP
