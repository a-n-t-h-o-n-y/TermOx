#ifndef CPPURSES_SYSTEM_EVENTS_MOUSE_HPP
#define CPPURSES_SYSTEM_EVENTS_MOUSE_HPP
#include <cppurses/system/events/input_event.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
class Widget;

/// Provides qualifying name for Mouse Event related classes.
struct Mouse {
    /// Standard mouse buttons to distinguish input.
    enum Button { None, Left, Middle, Right, ScrollUp, ScrollDown };

    /// Holds data from a Mouse Input Event.
    struct State {
        /// The mouse button used for the input event.
        Button button;

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
        Event(Input_event::Type type, Widget& receiver, const State& data);

       protected:
        const State data_;
    };

   public:
    /// Mouse Press Event.
    struct Press : Mouse::Event {
        Press(Widget& receiver, const State& data);
        bool send() const override;
        bool filter_send(Widget& filter) const override;
    };

    /// Mouse Release Event.
    struct Release : Mouse::Event {
        Release(Widget& receiver, const State& data);
        bool send() const override;
        bool filter_send(Widget& filter) const override;
    };

    /// Mouse Double Click Event. Not currently implemented.
    struct Double_click : Mouse::Event {
        Double_click(Widget& receiver, const State& data);
        bool send() const override;
        bool filter_send(Widget& filter) const override;
    };

    /// Mouse Wheel Event. Not currently implemented.
    struct Wheel : Mouse::Event {
        Wheel(Widget& receiver, const State& data);
        bool send() const override;
        bool filter_send(Widget& filter) const override;
    };

    /// Mouse Move Event. Not currently implemented.
    struct Move : Mouse::Event {
        Move(Widget& receiver, const State& data);
        bool send() const override;
        bool filter_send(Widget& filter) const override;
    };
};
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_MOUSE_HPP
