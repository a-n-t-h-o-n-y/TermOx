# Run an application and schedule timers

`Application` borrows a root widget, manages focus and input, and opens a terminal
when `run()` is called. Keep the root alive for the application's lifetime. Each
widget belongs to one application.

```cpp
#include <ox/ox.hpp>

int main()
{
    auto editor = ox::LineEdit{};
    auto app = ox::Application{editor};
    return app.run({.mouse_mode = ox::MouseMode::Move});
}
```

Use mouse move reporting for hover effects. The application initially focuses the
first visible, enabled Tab/Strong widget. Use `app.set_focus(widget)` or
`app.clear_focus()` for explicit changes; `app.focus().get()` returns a tracked view.

## Dispatch and repaint

`app.dispatch(event)` delivers an event immediately and applies its effects.
`app.post(event)` queues it for a later step. Handling an event does not itself
request painting: custom handlers return `EventResponse` with independent
`handled`, `repaint`, and optional `quit` fields. See
[custom widget input](widget-api.md#input-and-effects) for an implementation.

Widget setters schedule their documented effects. After changing external drawing
state, use `app.request_repaint()` on the application thread. Use `app.quit(code)`
to exit. Keep widget mutations, focus, timer commands, and painting on that thread;
[background work](background-work.md) uses `post` to return an owned result safely.

## Timers

Retain a `Timer` handle for as long as it should run. A widget timer receives the
current widget by reference and follows its moves:

```cpp
#include <ox/ox.hpp>
#include <chrono>

int main()
{
    auto elapsed = ox::IntegerLabel{{.value = 0}};
    auto app = ox::Application{elapsed};
    auto timer = ox::Timer{
        elapsed, std::chrono::seconds{1},
        [](ox::IntegerLabel& label) {
            label.set_value(label.value + 1);
            return ox::EventResponse{};
        },
        true,
    };
    return app.run();
}
```

The final `true` starts this timer; widget timers are stopped by default. `start()`
starts or restarts it, `stop()` cancels its deadline, and destroying the handle
cancels its schedule. Intervals must be positive. Callbacks run on the application
thread, so they must not block on I/O or long calculations.

The application adopts widget timers, including those created after attachment.
Hidden or disabled targets are skipped. Detachment stops and releases their timers;
destruction expires their tracked targets. For scheduling without a widget, create
`Timer(interval, callback)` and register it with `EventLoop::watch()`.

Timers share the event loop rather than creating threads. A late step delivers one
tick and keeps the original cadence instead of replaying missed ticks. For purely
visual transitions, use [Animation](theme-and-motion.md#motion).

## Headless dispatch

Construction, `step()`, and `paint()` need no terminal. Tests can post a resize and
input, step the application, and inspect the resulting frame:

```cpp
auto editor = ox::LineEdit{};
auto app = ox::Application{editor};
app.post(esc::Resize{{20, 1}});
app.post(ox::KeyEvent{.text = U"hello"});
app.step();
app.paint();
// Inspect app.loop().screen.
```

For dispatch batches, wakeups, waiting, and driving `EventLoop`/`Terminal` without
widgets, see [event-loop internals](internals/event-loop.md). For propagation,
modal scopes, pointer capture, and focus recovery, see
[routing internals](internals/routing-and-focus.md).
