# Write a custom widget

Derive from `Widget`, handle input, and paint local content:

```cpp
#include <ox/ox.hpp>

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

int main()
{
    auto counter = Counter{};
    return ox::Application{counter}.run();
}
```

## Input and effects

Presses, repeats, and releases reach the same key overload with their original action.
The application consumes Tab and Shift-Tab presses/repeats for focus navigation;
Tab releases route normally. Paste uses `handle(esc::PasteChunk const&)`.
Hover and focus lifecycle notifications keep `mouse_enter`, `mouse_leave`, `focus_in`,
and `focus_out`, each returning `EventResponse`.

`EventResponse` is a plain aggregate: `handled` stops bubbling, `repaint` requests a
frame, and `quit` requests an exit code. `combine(a, b)` combines these values; the
first quit wins. Raw `handle` calls and core `dispatch` return effects without applying
them. The event-loop boundary applies returned effects. For direct dispatch,
focus commands, and worker posting, see [Application and timers](event-loop.md).

## State and geometry

`visible`, `enabled`, `focus_policy`, `width_policy`, and `height_policy` are public configuration.
Invisible widgets hide their subtree. Disabled widgets remain visible but their
subtree receives no input, focus, or widget timer callbacks.

`bounds()`, `at()`, and `size()` return parent-local geometry by value.
`is_focused()` reads the focus state; application focus transitions are its only writer.

`arrange` validates dimensions, updates bounds, requests painting if attached, and calls
`on_arrange(previous_bounds)`. It calls the hook even when bounds are unchanged, so
layout can react to changed content or policy. Parents arrange their children
through the same function. Validation failure leaves geometry intact; a throwing
hook leaves the new geometry in place. See
[layout internals](internals/layout.md#refresh-and-callback-mutations) for reentrant
arrangement and failure behavior.

<a id="measurement-containers-and-capture"></a>

## Measurement and refresh

`measure(Area available, esc::WidthPolicy)` returns natural content size without changing
geometry. Width and height policies independently constrain allocation; `refresh()`
requests layout and repaint after public configuration changes. Text/content commands
request these effects automatically. `glyph_width_policy()` reads the attached frame's
Unicode width policy. `is_enabled()` includes ancestor input eligibility when attached.

## Pointer capture

Use `capture_pointer(button)` to keep receiving motion and matching release outside a
widget's bounds, and `release_pointer()` to end the gesture. The application cancels
capture when its owner becomes hidden, disabled, detached, or unavailable. Override
`pointer_cancel()` to clear gesture state. Capture and the root follow widget moves.

## Painting

`paint(Canvas)` returns an optional cursor in local coordinates. Return an empty cursor
when the widget has none. The application uses only the focused widget's cursor,
clips it, and translates it into terminal coordinates. Canvas supplies
local size, clipping, and width policy.
The order is `paint_background`, `paint`, children, then `paint_overlay`.
Fill a container background in the first hook and draw overlays in the last.

## Editing bindings

Use `TextEdit`, `TextLayout`, and `<ox/text_view.hpp>` functions for a
custom editor. They return input effects and share aligned caret, hit-test, and
painting geometry. [Text editing internals](internals/text.md#text-view-composition)
shows their composition; [Text editing](text-editing.md) covers built-in widgets.

## Timers and lifetime

Use [Application and timers](event-loop.md#timers) for retained widget timers and
scheduling. Timer callbacks receive the current widget instance, so they can use the
same state-changing methods as input handlers.

`track(widget)` returns a move-aware weak view. `view.try_get()` returns a pointer or
null without throwing; `view.get()` throws when unavailable. Typed views also reject
a target whose derived type was lost through slicing. Use `tracked` or `Connection`
to apply these checks to callbacks; [Connections and lifetime](connections.md)
explains borrowing and callback ownership.

`get_children()` returns a borrowed range of children for both tuple and
dynamic-container composites.

See [Custom drawing](drawing.md) for shapes, styles, and pixels, or the
[guides index](README.md) for built-in widgets.
