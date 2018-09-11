#ifndef CPPURSES_WIDGET_CURSOR_DATA_HPP
#define CPPURSES_WIDGET_CURSOR_DATA_HPP
#include <cstddef>

#include <signals/signal.hpp>

#include <cppurses/widget/point.hpp>

namespace cppurses {
class Widget;

/// Holds and provides access to all data relevant to a Widget's cursor.
class Cursor_data {
   public:
    Cursor_data(const Widget* owner);

    /// Query if the cursor is enabled.
    bool enabled() const;

    /// Get the local x coordinate of the cursor.
    std::size_t x() const;

    /// Get the local y coordinate of the cursor.
    std::size_t y() const;

    /// Get the local position of the cursor.
    Point position() const;

    /// Enable the cursor(show on screen).
    void enable(bool enable = true);

    /// Disable the cursor(do not show on screen).
    void disable(bool disable = true);

    /// Enables the cursor if disabled, or disables if enabled.
    void toggle();

    /// Set the local x coordinate of the cursor.
    void set_x(std::size_t x);

    /// Set the local y coordinate of the cursor.
    void set_y(std::size_t y);

    /// Set the local position of the cursor.
    void set_position(Point p);

    /// Signal called when the cursor is moved, passing along the new position.
    sig::Signal<void(Point)> moved;

   private:
    Point position_{0, 0};
    bool enabled_{false};
    const Widget* owner_;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_CURSOR_DATA_HPP
