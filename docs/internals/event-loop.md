# Event-loop internals

[Core concepts](../core-concepts.md) covers normal application use;
[Background work](../recipes.md#background-work) covers thread ownership and posting results.

## Dispatch batches

`EventLoop::step(handler, now)` processes a snapshot of posted events, then due timers
and animations. Callback posts wait for the next step. Input order is preserved;
repaint requests coalesce into one pending frame. Quit stops the batch and suppresses
painting. `paint(handler)` clears and paints once when a frame is pending. For an
`Application`, clearing applies the theme background in the same pass. Resize
resets frame storage before calling a handler's resize overload, if present.

## Waiting and terminal input

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
