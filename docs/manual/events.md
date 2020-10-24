# Events

An Event is an action that is applied to a specific Widget, event handler
methods within the `Widget` class are called on by the library to respond to
each event type. Each event handler has a cooresponding [signal](signals.md)
that is emitted after the function handler has been called.

## Event Handlers

Returning `true` from an event handler tells the event system that the event was
successfully processed. Returning `false` for some event types will send the
event to the parent Widget. Event handler implementations should call down to
the base class' implementation before returning.

Below is a listing of all event handlers. These are virtual member functions of
`Widget`. The Signal below each method is emitted after the event handler has
been called.

### Keyboard Event

```cpp
/// Sent to whichever Widget is in focus when the key is pressed.
bool key_press_event(Key k);
Signal<void(Key)> key_pressed;
```

*See [Key](key.md) Enum*

### Mouse Events

```cpp
/// Left, Middle, or Right mouse button was just pressed somewhere on this Widget.
bool mouse_press_event(Mouse const& m);
Signal<void(Mouse const&)> mouse_pressed;

/// Left, Middle, or Right mouse button was just released somewhere on this Widget.
bool mouse_release_event(Mouse const& m);
Signal<void(Mouse const&)> mouse_released;

/// Left, Middle, or Right mouse button was just double-clicked somewhere on this Widget.
bool mouse_double_click_event(Mouse const& m);
Signal<void(Mouse const&)> mouse_double_clicked;

/// The mouse wheel was either scrolled up or down somewhere on this Widget.
bool mouse_wheel_event(Mouse const& m);
Signal<void(Mouse const&)> mouse_wheel_scrolled;

/// Sent when the mouse passes anywhere over this Widget.
/** Not Implemented Yet */
bool mouse_move_event(Mouse const& m);
Signal<void(Mouse const&)> mouse_moved;
```

*See [Mouse](mouse.md) Struct*

### Focus Events

```cpp
/// Sent when this Widget gains focus.
bool focus_in_event();
Signal<void()> focused_in;

/// Sent when this Widget looses focus.
bool focus_out_event();
Signal<void()> focused_out;
```

### Widget Status Events

```cpp
/// Widget was just enabled and is able to display itself on the screen.
bool enable_event();
Signal<void()> enabled;

/// Widget was just disabled and is no longer being displayed.
bool disable_event();
Signal<void()> disabled;

/// This Widget was removed from its parent and is scheduled to be deleted after this event is processed.
bool delete_event();
Signal<void()> destroyed;
```

### Display Events

```cpp
/// Widget was just given new coordinates.
/** Both Points are given as global coordinates from the top left of the
 *  terminal window to the top left of the Widget. */
bool move_event(Point new_position, Point old_position);
Signal<void(Point const&, Point const&)> moved;

/// Widget was just given a new height and/or width.
bool resize_event(Area new_size, Area old_size);
Signal<void(Area const&, Area const&)> resized;

/// Widget needs to be re-drawn.
/** The implementation should call on Painter to paint to the screen. */
bool paint_event();
Signal<void()> painted;
```

### Timer Event

```cpp
/// Animation system uses this to notify a Widget to animate itself.
bool timer_event();
Signal<void()> timer;
```

### Child Events

*Only Layouts should override these methods.*

```cpp
/// A child Widget has been added to this Layout.
/** `child` is a reference to the newly added Widget. */
bool child_added_event(Widget& child);
Signal<void(Widget&)> child_added;

/// A child Widget is removed from this Layout.
/** `child` is a reference to the removed Widget. */
bool child_removed_event(Widget& child);
Signal<void(Widget&)> child_removed;

/// A child Widget's Size_policy is altered.
/** `child` is a reference to the modified Widget. */
bool child_polished_event(Widget& child);
Signal<void(Widget&)> child_polished;
```

## Event Filters

Events may be intercepted by other Widgets if they install themselves as an
event filter on the receiver. Once installed, all events bound for the receiving
Widget will first be sent to the event filter Widget via it's event filter
handler.

Filters are installed on a Widget via the `void
Widget::install_event_filter(Widget& filter)` member function. The `filter`
Widget will now receive all Events bound for the Widget it was installed on.
These events are sent to the `..._filter(...)` member functions.

Event handlers are sent a reference to the Widget they were originally bound for
as the first parameter, the event's parameters follow.  If the event filter
handler returns `true`, the event will not be sent to the receiving Widget, or
any other Event filters. Events are handed to the filters in order of
installation, then to the recieving Widget if no filters handle the Event.

The following are virtual member functions of `Widget`:

```cpp
bool key_press_event_filter(Widget& receiver, Key const& k);
bool mouse_press_event_filter(Widget& receiver, Mouse const& m);
bool mouse_release_event_filter(Widget& receiver, Mouse const& m);
bool mouse_double_click_event_filter(Widget& receiver, Mouse const& m);
bool mouse_wheel_event_filter(Widget& receiver, Mouse const& m);
bool mouse_move_event_filter(Widget& receiver, Mouse const& m);
bool focus_in_event_filter(Widget& receiver);
bool focus_out_event_filter(Widget& receiver);
bool enable_event_filter(Widget& receiver);
bool disable_event_filter(Widget& receiver);
bool delete_event_filter(Widget& receiver);
bool move_event_filter(Widget& receiver, Point new_position, Point old_position);
bool resize_event_filter(Widget& receiver, Area new_size, Area old_size);
bool paint_event_filter(Widget& receiver);
bool timer_event_filter(Widget& receiver);
bool child_added_event_filter(Widget& receiver, Widget& child);
bool child_removed_event_filter(Widget& receiver, Widget& child);
bool child_polished_event_filter(Widget& receiver, Widget& child);
```

## Custom Events

A custom Event can be created with an instance of the `Custom_event` type.

```cpp
struct Custom_event {
    std::function<void()> send;
    std::function<bool()> filter_send = [] { return false; };
};
```

The `send` member will be called on when the Event is sent, and the
`filter_send` member is called when filtering is required. The `Custom_event`
can be sent immediately with `System::send_event` or added to the Event queue
with `System::post_event`.

An example of how to use the `Custom_event` type:

```cpp
auto my_custom_event(My_widget& receiver, std::string data) -> Custom_event
{
    return {[&reciever, data] { receiver.custom_event_handler(data); }};
}

// Later...
System::post_event(my_custom_event(widg, "some data"));
```

## See Also

- [Event Loop](event-loop.md)
- [System](system.md)
