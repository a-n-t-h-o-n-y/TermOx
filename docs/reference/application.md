# Application and widget core

[API reference](../reference.md) · [Guides](../README.md)

Application ownership, widget contracts, callbacks, focus, and scheduling.

## On this page

- [ox::Application](#oxapplication)
- [ox::Widget](#oxwidget)
- [ox::Connection](#oxconnection)
- [ox::LifetimeView](#oxlifetimeview)
- [ox::FocusPolicy](#oxfocuspolicy)
- [ox::Focus](#oxfocus)
- [Routing and focus](#routing-and-focus)
- [ox::Timer](#oxtimer)
- [ox::EventLoop](#oxeventloop)

## ox::Application

[`#include <ox/application.hpp>`](../../include/ox/application.hpp)

An application owns its event loop, focus, and widget interaction. Construction does
not open a terminal. The root widget must outlive the application.

```cpp
auto app = Application{head};
return app.run({.mouse_mode = MouseMode::Move});
```

`step(now)` dispatches posted events and due timers without terminal I/O. `paint()`
paints a pending frame; `loop().screen` contains its cells. `post(event)` accepts events
from background threads. `request_repaint()` and `quit(code)` operate on the application
thread. `dispatch(event)` delivers immediately and applies effects; raw `handle`
methods return effects. `set_focus(widget)` and `clear_focus()` are commands;
`focus()` provides read-only access to the manager.

See [the event loop guide](../event-loop.md) for the response contract and scheduling.

## ox::Widget

[`#include <ox/widget.hpp>`](../../include/ox/widget.hpp)

A move-only base for input and drawing. Configuration is public; geometry, focus,
and lifetime ownership are controlled by the core. See [the widget API guide](../widget-api.md)
for a complete example.

```cpp
FocusPolicy focus_policy;
SizePolicy width_policy;
SizePolicy height_policy;
bool visible = true;
bool enabled = true;

Rect bounds() const;
Point at() const;
Area size() const;
bool is_focused() const;

virtual auto handle(KeyEvent const&) -> EventResponse;
virtual auto handle(MousePress const&) -> EventResponse;
virtual auto handle(MouseRelease const&) -> EventResponse;
virtual auto handle(MouseMove const&) -> EventResponse;
virtual auto handle(MouseWheel const&) -> EventResponse;
virtual auto handle(esc::PasteChunk const&) -> EventResponse;
virtual auto paint(Canvas) -> Cursor;

// Hover and focus notifications return EventResponse:
virtual auto mouse_enter() -> EventResponse;
virtual auto mouse_leave() -> EventResponse;
virtual auto focus_in() -> EventResponse;
virtual auto focus_out() -> EventResponse;

virtual auto get_children() -> zzz::Generator<Widget&>;
virtual auto get_children() const -> zzz::Generator<Widget const&>;

// Protected layout hook; bounds have already changed:
virtual void on_arrange(Rect previous);
```

Default input and lifecycle handlers return an empty response; default paint returns
no cursor. Press, repeat, and release actions use the same key handler. Use
`using Widget::handle;` when a derived class needs direct access to inherited overloads.

`arrange(widget, bounds)` validates dimensions, updates parent-local geometry,
requests painting if attached, and calls the hook even when bounds are unchanged.
Parents arrange children with this function. Paint returns a local cursor; only the
focused widget's returned cursor is presented. No persistent cursor field is stored.

Public configuration and widget-specific state can be edited directly. Arrange the
parent again after changing layout policies. Request painting after changes that are
not detected by tree synchronization. Geometry and focus cannot be written directly.
Move operations transfer lifetime tracking; assignment invalidates the overwritten
identity. `track(widget)` provides safe lookup without exposing lifetime storage.

## ox::Connection

[`#include <ox/widget.hpp>`](../../include/ox/widget.hpp)

```cpp
auto id = ox::Connection{
    .signal = numberpad.on_press,
    .slot = [](int n, auto& total_label) { total_label.value += n; },
}(total_label);
// Optional explicit disconnection:
numberpad.on_press.disconnect(id);
```

`Connection{.signal = signal, .slot = callback}(widgets...)` connects a signal to a callback.
Signal arguments come first, followed by references to tracked widgets. Tracking borrows
identity, follows moves, permits const targets, and suppresses invocation after
destruction, overwrite, or slicing. Empty callbacks are rejected before subscription.
Calls and widget mutations belong to the application thread.

The signal owns its callback. Discarding the returned `sl::Identifier` leaves the
connection active; it is not an RAII handle. The callback's captures remain owned until
the signal disconnects the slot or is destroyed, even when a tracked target expires.
Explicit disconnection returns the slot; retaining that slot retains its captures.
Constructing the configuration does not connect it; invoke it immediately to subscribe.

## ox::LifetimeView

[`#include <ox/widget.hpp>`](../../include/ox/widget.hpp)

`track(widget)` creates a weak view that follows moves and expires on destruction or
overwrite. Storage is private. `try_get()` returns a typed pointer or null without
throwing; `get()` returns a reference or throws `std::logic_error`; `valid()` tests
availability. Typed views reject a target whose derived type was lost through slicing.

```cpp
auto view = track(widget);
if (auto* target = view.try_get()) { target->enabled = false; }
```

`tracked(callback, widgets...)` invokes a callback only while every target is valid,
passing tracked widgets after the invocation's ordinary arguments. `Connection`
combines this with signal connection. Views and widget mutation belong to the
application thread.

## ox::FocusPolicy

[`#include <ox/widget.hpp>`](../../include/ox/widget.hpp)

Provides a description of how a Widget can attain Focus. Focus allows a Widget to
receive key press events. Used by the Application class on mouse clicks and tab presses
to determine the next focus Widget.

```cpp
TextBox{} | FocusPolicy::Strong; // Enable editing in a TextBox by providing Focus.
```

The pipe (`|`) operator is overloaded for Widget and FocusPolicy to set the FocusPolicy
inline with Widget construction.

```cpp
enum class FocusPolicy : std::uint8_t { None, Tab, Click, Strong };
```

- **None**: The Widget will never receive Focus.
- **Tab**: A tab or shift + tab key press will give it Focus, if it is next in order for
Tab Focus.
- **Click**: A left mouse click on the Widget will give it Focus.
- **Strong**: Both left mouse click and tab key press will give it Focus.

## ox::Focus

[`#include <ox/focus.hpp>`](../../include/ox/focus.hpp)

`Focus(root)` manages one tree and returns transition effects without applying them.
`app.set_focus(widget)` and `app.clear_focus()` apply those effects at the application
boundary. `app.focus().get()` returns a lifetime view; `Widget::is_focused()` reads
focus state. See [routing and focus](../routing-and-focus.md).

## Routing and focus

[`#include <ox/routing.hpp>`](../../include/ox/routing.hpp)

`widget_tree(root)` returns a snapshot with ancestry, clipped bounds, lifetime views,
and input eligibility. `hit_path(tree, point)` returns mouse ancestry;
`focus_order(tree)` returns visible Tab/Strong candidates. `Focus(root)` owns
selection for one tree. Widget input, hover, focus, and timer callbacks return
`EventResponse`; mouse and keyboard input bubble until handled or quitting.

See [routing and focus](../routing-and-focus.md) for ownership, hover transitions,
Tab behavior and callback mutations.

## ox::Timer

[`#include <ox/timer.hpp>`](../../include/ox/timer.hpp)

A movable timer handle using the event loop's steady-clock scheduler. Timers create no
threads. Retain the handle to retain its schedule.

```cpp
Timer(std::chrono::milliseconds interval,
      std::function<EventResponse()> callback,
      bool launch = true);
// For any derived widget W and callback returning EventResponse:
Timer(W& widget, std::chrono::milliseconds interval, Callback callback,
      bool launch = false);
```

Widget callbacks receive `W&` at execution time, follow moves, and are adopted by the
application. Register generic callback timers with `loop.watch(timer)`. Intervals must
be positive and callbacks nonempty. `start(now)` resets the deadline; `stop()` cancels
it. `is_running()` reports whether a deadline exists. Delays deliver one tick and
skip missed ticks. Multiple widget timers have independent callbacks.

## ox::EventLoop

[`#include <ox/core/event_loop.hpp>`](../../include/ox/core/event_loop.hpp)

Owns queued events, scheduled timers, frame storage, repaint state, and the exit code.
`post()` is thread safe. `step(handler, now)` and `paint(handler)` run without terminal
I/O. `wait(terminal.wait_state())` waits for terminal readiness, queued posts, or the
next deadline without consuming input. `wait()` can also wait without a terminal.
`process_events(loop, terminal, handler)` drives the same core with terminal input.
