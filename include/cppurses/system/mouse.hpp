#ifndef CPPURSES_SYSTEM_MOUSE_HPP
#define CPPURSES_SYSTEM_MOUSE_HPP
#include <cppurses/widget/point.hpp>

namespace cppurses {

/// Mouse related data generated from Mouse Events.
struct Mouse {
    /// Standard mouse buttons to distinguish input.
    enum class Button { Left, Middle, Right, ScrollUp, ScrollDown };

    /// The mouse button that created the event.
    Button button;

    /// Keyboard modifiers
    struct Modifiers {
        bool shift = false;
        bool ctrl  = false;
        bool alt   = false;
    } modifiers;

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
#endif  // CPPURSES_SYSTEM_MOUSE_HPP
