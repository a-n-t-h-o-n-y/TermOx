# Mouse Events

Mouse events are sent to the Widget that the mouse was over at the time of the
event. This is the Widget 'furthest' down the Widget tree, so Layouts that
contain Widgets will have the mouse event sent to one of their child Widgets,
instead of the Layout. They contain Widget local coordinates and global terminal
coordinates.

## Event Handlers

```cpp
class Widget {
   protected:
    ...
    // Called on...

    // ... any mouse button pressed down.
    virtual bool mouse_press_event(Mouse::State const& mouse);

    // ... any mouse button released.
    virtual bool mouse_release_event(Mouse::State const& mouse);

    // ... any mouse button double click.
    virtual bool mouse_double_click_event(Mouse::State const& mouse);

    // ... scroll wheel movement, button will be either ScrollUp or ScrollDown.
    virtual bool mouse_wheel_event(Mouse::State const&  mouse);

    // Not Implemented Yet
    virtual bool mouse_move_event(Mouse::State const& mouse);
    ...
};

struct Mouse {
    ...
    struct State {
        // The mouse button used for the input event.
        Button button;

        // Keyboard keys held down when mouse event occurred.
        struct Modifiers {
            bool shift = false;
            bool ctrl  = false;
            bool alt   = false;
        } modifiers;

        // The terminal screen global coordinate of the input event.
        /** Top left of screen is (x:0, y:0). */
        Point global;

        // The Widget local coordinate of the input event.
        /** Top left of Widget is (x:0, y:0). */
        Point local;

        // Input device's ID.
        short device_id;
    };
    ...
};
```

## Signals

```cpp
class Widget {
    ...
   public:
    Signal<void(Mouse::State const&)> mouse_pressed;
    Signal<void(Mouse::State const&)> mouse_released;
    Signal<void(Mouse::State const&)> mouse_double_clicked;
    Signal<void(Mouse::State const&)> mouse_wheel_scrolled;
    Signal<void(Mouse::State const&)> mouse_moved;  // Not Implemented Yet
    ...
};
```

## Event Filters

```cpp
class Widget {
    ...
   protected:
    virtual bool mouse_press_event_filter(Widget& receiver, Mouse::State const& mouse);

    virtual bool mouse_release_event_filter(Widget& receiver, Mouse::State const& mouse);

    virtual bool mouse_double_click_event_filter(Widget& receiver, Mouse::State const& mouse);

    virtual bool mouse_wheel_event_filter(Widget& receiver, Mouse::State const& mouse);

    virtual bool mouse_move_event_filter(Widget& receiver, Mouse::State const& mouse);
    ...
};
```
