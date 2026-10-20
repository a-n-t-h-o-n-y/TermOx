# Core concepts

A TermOx application has a tree of widgets. Layouts arrange its children, signals
connect actions to state changes, and `Application` handles input and painting.

## Widgets and ownership

Layouts store widgets by value, or borrow them through references:

```cpp
auto name = ox::LineEdit{};
auto save = ox::Button{{.label = {.text = "Save"}}};
auto row = ox::Row{std::tie(name, save)};
```

Here, `row` borrows `name` and `save`; both must outlive it. `Row{name, save}` would
store copies instead. Moving widgets into a layout transfers their state.
`children` exposes the stored tuple or container, and `get_child<I>(layout)` accesses
a fixed tuple child.

`Application` borrows its root widget. Keep the root and any borrowed descendants
alive until the application is destroyed. Each widget belongs to one application.

## Connections and lifetime

Use `Connection` to pass a widget to a signal callback:

```cpp
#include <ox/ox.hpp>

int main()
{
    auto button = ox::Button{{.label = {.text = "Run"}}};
    auto status = ox::Label{"Ready"};
    auto root = ox::Column{std::tie(button, status)};
    ox::Connection{.signal = button.on_activate,
                   .slot = [](ox::Label& label) { label.set_text("Running"); }}(status);
    return ox::Application{root}.run();
}
```

The `(status)` invocation subscribes the callback; constructing `Connection` alone
does not. Signal arguments come first, followed by references to tracked widgets.
The tracked reference follows widget moves, and the callback is skipped if the
widget is destroyed. Pass widgets this way rather than capturing their addresses
or references to their current locations.

The signal owns the callback and its captures. The returned identifier allows
explicit disconnection; discarding it does **not** disconnect. For example:

```cpp
#include <ox/widget.hpp>

sl::Signal<void(int)> changed;
ox::Widget target;
auto id = ox::Connection{
    .signal = changed,
    .slot = [](int value, ox::Widget& widget) { widget.enabled = value != 0; },
}(target);
changed(1);
changed.disconnect(id);
```

Capturing other application objects still requires those objects to outlive the
callback. All signal emission and widget mutation belong on the application thread.
See [Connection](https://a-n-t-h-o-n-y.github.io/TermOx/structox_1_1_connection.html) for disconnection and signal-emission details.

For work that does not use a signal, `track(widget)` returns a weak view and
`tracked(callback, widgets...)` wraps a callback with lifetime checks. A weak view's
`try_get()` returns null if the target is gone; `get()` throws. Neither owns the widget.

## Updating widgets

Use setters such as `set_text` and `set_value` to change content. They request the
needed layout and repaint. Direct public field edits are not observed automatically:
call `refresh()` after changing configuration such as spacing, visibility, or styles.
When changing a child's size or visibility, refresh its parent layout.

For drawing state outside widgets, call `app.request_repaint()`. Custom event
handlers return [input effects](custom-widgets.md#input-and-effects) to request
painting or quit. Handling an event alone does not request a repaint.

## Running and focus

`app.run()` opens the terminal and processes events until the application exits.
Use `app.quit(code)` to exit from a callback. Enable mouse move reporting with
`app.run({.mouse_mode = ox::MouseMode::Move})` when using hover effects.

The application initially focuses the first visible, enabled Tab/Strong widget.
Tab and Shift-Tab move focus. Use `app.set_focus(widget)` or `app.clear_focus()` for
explicit changes. Invisible widgets hide their subtrees; disabled widgets stay
visible but their subtrees receive no input, focus, or widget timer callbacks.

`app.dispatch(event)` delivers an event immediately and applies its effects.
`app.post(event)` queues it for a later step. Only `post` is thread-safe; keep focus,
widget changes, timers, and painting on the application thread. See
[background work](recipes.md#background-work) for posting worker results safely.

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

The final `true` starts the timer; widget timers are stopped by default. Destroying
the handle cancels it. Callbacks run on the application thread and must not block.
Hidden or disabled targets are skipped; detachment stops and releases their timers.

Timers share the event loop rather than creating threads. Late steps deliver one
tick instead of replaying missed ticks. For visual transitions, use
[Animation](appearance.md#motion). See [Timer](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_timer.html) for scheduling options.

Continue with [Layout](layout.md) to arrange and resize widgets.
