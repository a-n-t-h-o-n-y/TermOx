#ifndef CPPURSES_SYSTEM_EVENTS_MOUSE_HPP
#define CPPURSES_SYSTEM_EVENTS_MOUSE_HPP
#include <cppurses/system/events/input_event.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
class Widget;

/// Provides qualifying name for Mouse Event related classes.
struct Mouse {
    /// Standard mouse buttons to distinguish input.
    enum class Button { None, Left, Middle, Right, ScrollUp, ScrollDown };

    /// Holds data from a Mouse Input Event.
    struct State {
        /// The mouse button used for the input event.
        Button button;

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

   private:
    /// Mouse Event Base Class.
    struct Event : Input_event {
        Event(Input_event::Type type, Widget& receiver, State const& data)
            : Input_event{type, receiver}, data_{data}
        {}

       protected:
        State const data_;
    };

   public:
    /// Mouse Press Event.
    struct Press : Mouse::Event {
        Press(Widget& receiver, State const& data)
            : Mouse::Event{Input_event::MouseButtonPress, receiver, data}
        {}
        auto send() const -> bool override;
        auto filter_send(Widget& filter) const -> bool override;
    };

    /// Mouse Release Event.
    struct Release : Mouse::Event {
        Release(Widget& receiver, State const& data)
            : Mouse::Event{Input_event::MouseButtonRelease, receiver, data}
        {}
        auto send() const -> bool override;
        auto filter_send(Widget& filter) const -> bool override;
    };

    /// Mouse Double Click Event. Not currently implemented.
    struct Double_click : Mouse::Event {
        Double_click(Widget& receiver, State const& data)
            : Mouse::Event{Input_event::MouseButtonDblClick, receiver, data}
        {}
        auto send() const -> bool override;
        auto filter_send(Widget& filter) const -> bool override;
    };

    /// Mouse Wheel Event.
    struct Wheel : Mouse::Event {
        Wheel(Widget& receiver, State const& data)
            : Mouse::Event{Input_event::MouseWheel, receiver, data}
        {}
        auto send() const -> bool override;
        auto filter_send(Widget& filter) const -> bool override;
    };

    /// Mouse Move Event. Not currently implemented.
    struct Move : Mouse::Event {
        Move(Widget& receiver, State const& data)
            : Mouse::Event{Input_event::MouseMove, receiver, data}
        {}
        auto send() const -> bool override;
        auto filter_send(Widget& filter) const -> bool override;
    };
};
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_MOUSE_HPP
