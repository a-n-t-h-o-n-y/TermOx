#ifndef TERMOX_WIDGET_CURSOR_DATA_HPP
#define TERMOX_WIDGET_CURSOR_DATA_HPP
#include <signals_light/signal.hpp>

#include <termox/widget/point.hpp>

namespace ox {

/// Holds and provides access to all data relevant to a Widget's cursor.
class Cursor {
   public:
    /// Signal called when the cursor is moved, passing along the new position.
    sl::Signal<void(Point)> moved;

   public:
    /// Get the local position of the cursor.
    [[nodiscard]] auto position() const -> Point;

    /// Query if the cursor is enabled.
    [[nodiscard]] auto is_enabled() const -> bool;

    /// Enable the cursor(show on screen).
    void enable(bool enable = true);

    /// Disable the cursor(do not show on screen).
    void disable(bool disable = true);

    /// Enable the cursor if disabled, or disable it if enabled.
    void toggle();

    /// Set the local position of the cursor.
    void set_position(Point p);

   private:
    Point position_ = {0, 0};
    bool enabled_   = false;
};

}  // namespace ox
#endif  // TERMOX_WIDGET_CURSOR_DATA_HPP
