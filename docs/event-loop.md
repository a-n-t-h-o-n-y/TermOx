# Application and event loop

`Application` can dispatch events, run timers, manage focus, and paint a frame without
opening a terminal. `run()` opens an Escape session and drives input and rendering on
the calling thread.

```cpp
#include <ox/ox.hpp>
using namespace ox;

int main()
{
    auto editor = LineEdit{};
    auto app = Application{editor};
    return app.run({.mouse_mode = MouseMode::Move});
}
```

The root widget must outlive the application. Each widget belongs to one application.
The application selects the first active widget that accepts tab focus. Focus is local
to the application; use `app.focus().set(widget)`, `get()`, or `clear()`.

## Dispatch and repaint

A handler returns an explicit result:

```cpp
struct EventResponse {
    bool handled = false;
    bool repaint = false;
    std::optional<int> quit = std::nullopt;
};
```

Return `{.handled = true}` when an event was processed without changing the display.
Return `{.handled = true, .repaint = true}` to request a frame, or
`{.handled = true, .quit = 0}` to exit. Quit takes precedence over painting.

`EventLoop::step(handler, now)` processes a snapshot of posted events, then due timers.
Posts made during callbacks wait for the next step. Input order is preserved. Multiple
repaint requests are combined into one pending frame. `paint(handler)` calls
`handle_paint(Canvas)` once if a frame is pending, and returns whether to present it.
A resize requests a repaint even if the handler has no resize method.

The existing widget callbacks still return `void`, so the application conservatively
requests a frame after delivering widget input or a widget timer tick. A lower-level
handler can decide whether each event needs a repaint.

```cpp
auto editor = LineEdit{};
auto app = Application{editor};
app.post(esc::Resize{{20, 1}});
app.post(KeyEvent{.text = U"hello"});
app.step();
app.paint();
// app.loop().screen contains the painted frame. No terminal has been opened.
```

Use `app.request_repaint()` after an external state change and `app.quit(code)` to
request exit on the application thread. `post(Event)` is thread safe; background work
should post an `event::Custom` callback to make changes on the application thread.
Callbacks, focus, widget state, timer operations, and painting stay on that thread.

The terminal driver bounds reads to 16 milliseconds so posted background events remain
responsive. It drains at most 64 input events per batch, then dispatches and paints,
allowing timers to run between batches. This uses no input thread or timer threads.

## Timers

A timer is a movable handle containing a callback and a steady-clock deadline. There
is no thread per timer. Retain the handle for as long as the timer should exist.

```cpp
auto loop = EventLoop{};
auto timer = Timer{std::chrono::milliseconds{30}, [&] {
    // Update animation state here.
    return EventResponse{.handled = true, .repaint = true};
}};
loop.watch(timer);
```

`Timer(widget, interval, launch)` registers a passive widget timer. The application
adopts it when it attaches the widget tree. Timers created after attachment are
registered immediately. The widget's lifetime handle preserves the target when the
widget moves; a destroyed target is ignored. Detaching a widget stops and releases its
timers, allowing it to join another application.

Intervals must be positive. `start(now)` starts or restarts at `now + interval`.
`stop()` cancels the deadline. `loop.unwatch(timer)` stops and releases a timer.
Destroying the handle cancels its schedule. One timer
belongs to one event loop at a time. A late step delivers one tick and advances to the
next deadline on the original cadence, rather than executing a burst of missed ticks.
An injected `steady_clock::time_point` makes scheduling tests deterministic.

## Lower-level composition

`EventLoop`, `Terminal`, and a handler can be used without `Application` or `Widget`:

```cpp
auto loop = EventLoop{};
auto terminal = Terminal{};
// handler provides handle_key_press, handle_paint, etc. as needed.
return process_events(loop, terminal, handler);
```

The loop owns posted events, scheduling, frame storage, repaint state, and the exit
request. The terminal owns its Escape session and previous rendered frame. Application
adds widget routing and focus. No process-wide event queue, focus, timer registry, or
quit flag is used.
