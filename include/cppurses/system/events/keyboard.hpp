#ifndef CPPURSES_SYSTEM_EVENTS_KEYBOARD_HPP
#define CPPURSES_SYSTEM_EVENTS_KEYBOARD_HPP
#include <cppurses/system/events/input_event.hpp>

namespace cppurses {
class Widget;
enum class Key : short;

/// Provides qualifying name for Keyboard Event related classes.
struct Keyboard {
    /// Holds data from a Key Input Event.
    struct State {
        /// The keycode of the input.
        Key key;

        /// The symbol representing the keycode of the input.
        /** Is '\0' if keycode is not printable. */
        char symbol;
    };

    /// Keyboard Event Base Class.
    struct Event : Input_event {
        Event(Input_event::Type type, Widget& receiver, Key key_code);

       protected:
        const Key key_;
    };

    /// Key Press Event.
    struct Press : Keyboard::Event {
        Press(Widget& receiver, Key key_code);
        bool send() const override;
        bool filter_send(Widget& filter) const override;
    };

    // Key Release Event.
    struct Release : Keyboard::Event {
        Release(Widget& receiver, Key key_code);
        bool send() const override;
        bool filter_send(Widget& filter) const override;
    };
};
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_KEYBOARD_HPP
