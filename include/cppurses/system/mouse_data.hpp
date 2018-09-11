#ifndef CPPURSES_SYSTEM_MOUSE_DATA_HPP
#define CPPURSES_SYSTEM_MOUSE_DATA_HPP
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {

/// Holds data from a Mouse Input Event.
struct Mouse_data {
    /// The mouse button used for the input event.
    Mouse_button button;

    /// The terminal screen global coordinate of the input event.
    /** Top left of screen is (x:0, y:0). */
    Point global;

    /// The Widget local coordinate of the input event.
    /** Top left of Widget is (x:0, y:0). */
    Point local;

    /// Input device's ID.
    short device_id;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_MOUSE_DATA_HPP
