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
The application selects the first visible, enabled widget that accepts tab focus. Focus is local
to the application; use `app.set_focus(widget)`, `app.focus().get()`, or `app.clear_focus()`.

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
`paint(Canvas)` once if a frame is pending, and returns whether to present it.
A resize requests a repaint even if the handler has no resize method.

Widget input, hover, focus, and timer callbacks return `EventResponse`.
Raw handlers and standalone focus operations return effects; the event loop applies
them. `app.dispatch(event)` delivers immediately and applies effects. Handling an
event does not imply repainting. See [routing and focus](routing-and-focus.md) for
clipped mouse paths, keyboard bubbling, Tab navigation, and focus lifetime.

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

The terminal driver waits for terminal input, posted events, or the earliest timer,
parser, or resize-check deadline. Posting to an empty queue signals a nonblocking pipe
on POSIX or a manual-reset event on Windows; reset and queue transfer share the queue
lock so posts cannot lose their wakeup. There is no fixed polling interval. Escape's
default resize checks still wake an idle terminal about every 100 milliseconds.
The driver drains at most 64 input events per batch, then dispatches and paints,
allowing timers to run between batches. Terminal I/O stays on the calling thread.

For a custom driver, call `loop.wait(terminal.wait_state())`, then read terminal input
with a zero timeout. `wait()` never consumes input. `loop.wait()` without a terminal
waits for a post or scheduled timer, indefinitely when neither has a deadline. Only
`post()` is thread safe; request repaint, quit, and change timers on the dispatch thread.

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

`Timer(widget, interval, callback, launch)` registers a passive widget timer.
The callback receives the current widget by reference, so it follows widget moves. The application
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
// handler provides handle(KeyEvent const&), paint(Canvas), etc. as needed.
return process_events(loop, terminal, handler);
```

The loop owns posted events, scheduling, frame storage, repaint state, and the exit
request. The terminal owns its Escape session and previous rendered frame. Application
adds widget routing and focus. No process-wide event queue, focus, timer registry, or
quit flag is used.

Frame storage and clipped drawing are described in [frames and canvases](frame-and-canvas.md).
