# Key Press Event

A Keyboard Event is sent to whichever Widget was in focus when the key press
occurred. Tab keys are intercepted when the in focus Widget has a Tab Focus
Policy, and the Event does not reach the Widget.

## Event Handler

```cpp
class Widget {
    ...
   protected:
    // Called on key press
    virtual bool key_press_event(Key::State const& keyboard);
    ...
};

struct Key {
    ...
    struct State {
        // An enum containing all keys that can be pressed.
        Code key;

        // The symbol representing the keycode of the input.
        /** Is '\0' if keycode is not printable. */
        char symbol;
    };
    ...
};
```

## Signal

```cpp
class Widget {
    ...
   public:
    Signal<void(Key::State const&)> key_pressed;
    ...
};
```

## Event Filter

```cpp
class Widget {
    ...
   protected:
    virtual bool key_press_event_filter(Widget& receiver, Key::State const& keyboard);
    ...
};
```

## See Also

[Source File](../../../include/cppurses/system/events/key.hpp)
