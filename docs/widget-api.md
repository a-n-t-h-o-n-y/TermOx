# Widget API

Widgets expose ordinary configuration and compose with the same geometry, drawing,
text, and event functions used by standalone handlers. Application-owned state has
read-only access, and handlers return effects explicitly.

## On this page

- [State and geometry](#state-and-geometry)
- [Measurement and refresh](#measurement-and-refresh)
- [Input and effects](#input-and-effects)
- [Pointer capture](#pointer-capture)
- [Painting](#painting)
- [Editing bindings](#editing-bindings)
- [Timers and lifetime](#timers-and-lifetime)
- [Related guides](#related-guides)

## State and geometry

`visible`, `enabled`, `focus_policy`, `width_policy`, and `height_policy` are public configuration.
Invisible widgets hide their subtree. Disabled widgets remain visible but their
subtree receives no input, focus, or widget timer callbacks.

`bounds()`, `at()`, and `size()` return parent-local geometry by value.
`is_focused()` reads the focus state; application focus transitions are its only writer.

```cpp
arrange(widget, {{2, 1}, {20, 3}});
arrange(parent, parent.bounds()); // Recalculate layout after changing child policies.
```

`arrange` validates dimensions, updates bounds, requests painting if attached, and calls
`on_arrange(previous_bounds)`. It calls the hook even when bounds are unchanged, so
layout can react to changed content or policy. Validation failure leaves geometry
intact. If the hook throws, the new geometry remains; layout callbacks are not a
transaction across a whole subtree. Parents arrange their children through the same
function. Root positioning starts at the terminal origin.

`WidgetDerived` requires public inheritance and accepts const-qualified targets for borrowing. Widget's header
includes the drawing, events, geometry, and timer declarations it uses directly, rather
than relying on the core umbrella header to expose unrelated terminal machinery.

<a id="measurement-containers-and-capture"></a>

## Measurement and refresh

`measure(Area available, esc::WidthPolicy)` returns natural content size without changing
geometry. Width and height policies independently constrain allocation; `refresh()`
requests layout and repaint after public configuration changes. Text/content commands
request these effects automatically. `glyph_width_policy()` reads the attached frame's
Unicode width policy. `is_enabled()` includes ancestor input eligibility when attached.

## Input and effects

Override `handle(Input const&)` for the input types a widget uses:

```cpp
struct Counter : ox::Widget {
    using Widget::handle; // Keep default overloads available for direct calls.
    int value = 0;
    Counter() : Widget{ox::FocusPolicy::Strong} {}

    auto handle(ox::KeyEvent const& event) -> ox::EventResponse override
    {
        if (event.action == ox::KeyAction::Release) { return {}; }
        if (!event.matches(U' ', {}, event.action)) { return {}; }
        ++value;
        return {.handled = true, .repaint = true};
    }

    auto paint(ox::Canvas canvas) -> ox::Cursor override
    {
        ox::put(canvas, {0, 0}, std::to_string(value));
        return {};
    }
};
```

Presses, repeats, and releases reach the same key overload with their original action.
The application consumes Tab and Shift-Tab presses/repeats for focus navigation;
Tab releases route normally. Paste uses `handle(esc::PasteChunk const&)`.
Hover and focus lifecycle notifications keep `mouse_enter`, `mouse_leave`, `focus_in`,
and `focus_out`, each returning `EventResponse`.

`EventResponse` is a plain aggregate: `handled` stops bubbling, `repaint` requests a
frame, and `quit` requests an exit code. `combine(a, b)` combines these values; the
first quit wins. Raw `handle` calls and core `dispatch` return effects without applying
them. The event-loop boundary applies returned effects. Resize dispatch also resets
frame storage before calling the resize handler.

`app.dispatch(event)` delivers immediately and applies effects. `app.post(event)`
queues delivery for a later step and is thread safe. `app.set_focus(widget)` and
`app.clear_focus()` are explicit commands that apply transition effects;
`app.focus()` exposes a read-only manager. Standalone `Focus(root)` returns transition
effects without an event loop. Keep one focus manager per tree. Destruction releases
the focus flag without calling lifecycle hooks; call `clear()` explicitly to notify. `app.request_repaint()` and `app.quit(code)` are direct
commands on the application thread.

## Pointer capture

Use `capture_pointer(button)` to keep receiving motion and matching release outside a
widget's bounds, and `release_pointer()` to end the gesture. The application cancels
capture when its owner becomes hidden, disabled, detached, or unavailable. Override
`pointer_cancel()` to clear gesture state. Capture and the root follow widget moves.

## Painting

`paint(Canvas)` returns an optional cursor in local coordinates. Return an empty cursor
when the widget has none. There is no persistent widget cursor field. The application
uses only the focused widget's paint result, clips it, and translates it into terminal
coordinates. Canvas already supplies local size, clipping, and width policy.
The order is `paint_background`, `paint`, children, then `paint_overlay`.
Fill a container background in the first hook and draw overlays in the last.

## Editing bindings

`TextEdit` owns document state, selection, grapheme boundaries, and undo history.
`edit_key(edit, event, layout, validator)` supplies the default keyboard policy as a
free function. Custom bindings can call editing operations directly without subclassing
or modifying the document model. `TextLayout` owns the display layout independently. `<ox/text_view.hpp>` composes these
components with plain `Point` scroll state: `edit_text` handles a key or paste and
refreshes layout/scroll, `text_scroll` reveals a caret, `text_offset` and `text_position`
share the aligned geometry, and `paint_text` draws cached rows and selection. LineEdit
and TextBox share an internal implementation that composes these functions. See [text view composition](text-editing.md#text-view-composition).

## Timers and lifetime

Widget timers receive the actual widget instance at execution time:

```cpp
auto timer = ox::Timer{counter, std::chrono::milliseconds{30}, [](Counter& widget) {
    ++widget.value;
    return ox::EventResponse{.repaint = true};
}};
timer.start();
```

Retain the handle. Multiple timers can have independent callbacks. The application
adopts widget timers, including timers created after attachment. They follow moves,
respect ancestor input eligibility, and stop on detachment or destruction. Generic
`Timer(interval, callback)` composes directly with an event loop via `loop.watch(timer)`.
Callbacks and widget mutations run on the dispatch thread. Empty callbacks and
nonpositive intervals are rejected. A moved-from handle can be stopped or queried with
`is_running()`; `start()` and `duration()` reject it.

`track(widget)` returns a move-aware weak view. `view.try_get()` returns a pointer or
null without throwing; `view.get()` throws when unavailable. Typed views also reject
a target whose derived type was lost through slicing. Lifetime storage is private;
callers cannot replace it or forge a view from a shared pointer. `tracked` and
`Connection{.signal = signal, .slot = callback}(widgets...)` compose lifetime checks with signal callbacks.
The signal owns the slot; dropping its identifier keeps it connected. Tracked targets
are borrowed, and callback captures remain owned until explicit removal or signal
destruction. Empty callbacks are rejected.

Child enumeration remains a borrowed range through `get_children()`, supporting both
tuple and dynamic-container composites without imposing a new ownership model.

## Related guides

| Topic | Guide |
| --- | --- |
| Signal callbacks and tracked widget arguments | [Connections and lifetime](connections.md) |
| Layout containers and shared scrolling | [Container composition](containers.md) |
| Borders, dividers, and labels | [Decorations and layouts](decorations-layouts.md) |
| Buttons, checkboxes, and radio groups | [Controls](controls.md) |
| Tables and scrollbars | [Data tables](data-tables.md) |
| Editors and text views | [Text editing](text-editing.md) |
| Lists, choices, tabs, overlays, and menus | [Selection widgets](selection-and-application.md) |
| Values, trees, logs, forms, and files | [Composite widgets](value-and-composite-widgets.md) |
| Pixel images | [PixelGrid reference](reference/drawing.md#oxpixelgrid) |
