# Custom widgets

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

- [Input and effects](#input-and-effects)
- [Geometry and measurement](#geometry-and-measurement)
- [Pointer capture](#pointer-capture)
- [Painting](#painting)
- [Styles and glyphs](#styles-and-glyphs)
- [Pixel images](#pixel-images)
- [Scrollable widgets](#scrollable-widgets)
- [Headless testing](#headless-testing)

## Input and effects

Presses, repeats, and releases reach the same key overload with their original action.
The example handles Space presses and repeats. Tab and Shift-Tab presses/repeats are
reserved for application focus navigation. Paste uses `handle(esc::PasteChunk const&)`.
Override `mouse_enter`, `mouse_leave`, `focus_in`, or `focus_out` for lifecycle feedback.

`EventResponse::handled` stops bubbling, `repaint` requests a frame, and `quit` requests
an exit code. `combine(a, b)` combines effects; the first quit wins. Direct `handle`
calls and core `dispatch` return effects without applying them. `Application::dispatch`
applies them. See [Core concepts](core-concepts.md) for application state and lifetime.

## Geometry and measurement

`bounds()`, `at()`, and `size()` return parent-local geometry. Parents use `arrange`
to set child bounds; it requests painting and calls `on_arrange(previous_bounds)`,
even when bounds are unchanged. Use that hook to arrange children after content or
policy changes. `get_children()` returns a borrowed range of a composite's children.

`measure(available, glyph_width_policy)` reports natural content size without
changing state. Width and height policies constrain the allocation. Call `refresh()`
after public configuration changes, and refresh a nested layout when its children
change. Arrangement mutations must eventually settle.

See [Widget](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_widget.html) for hooks and geometry contracts, and
[layout internals](internals/layout.md#refresh-and-callback-mutations) for reentrant
arrangement and exception behavior.

## Pointer capture

Call `capture_pointer(button)` to keep receiving motion and matching release outside
widget bounds. Call `release_pointer()` when the gesture ends. The application cancels
capture if its owner becomes hidden, disabled, detached, or unavailable; override
`pointer_cancel()` to clear gesture state. Capture follows widget moves.

## Painting

The application owns the frame and passes a local, clipped Canvas to each widget.
Paint order is `paint_background`, `paint`, children, then `paint_overlay`.
Use the first hook for a container fill and the last for decorations over children.

Return a cursor in local coordinates from `paint`, or an empty cursor when there
is none. Only the focused widget's cursor is used; the application clips it and
translates it to terminal coordinates.

`canvas.size()` is the logical widget extent. `subcanvas(offset, size)` creates a
local view that cannot draw beyond its parent's clip. `put` clips complete Unicode
graphemes without wrapping. Keep application text as UTF-8; terminal columns are
not byte offsets. See [Canvas](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_canvas.html) for drawing operations.

## Styles and glyphs

`put` accepts UTF-8/UTF-32 strings, glyph ranges, and line/box shapes. `fill` accepts
a styled glyph, a character, or an appearance change. Character fills preserve
existing styles; style/color fills preserve text. `clear` uses default-styled spaces.

Glyph defaults fill unspecified colors and combine traits; an explicit patch applies
last. Use [theme roles](appearance.md#colors-and-overrides) for custom widgets that
follow the application theme.

Use `paint_border` and `paint_divider` to draw decorations directly. `put_label`
leaves the surrounding area untouched; `paint_label` clears its view first.
For a themed line in a widget's paint function:

```cpp
ox::paint_rule(canvas, 2.5, 8.0,
               appearance({}, ox::Surface::Accent),
               appearance({}, ox::Surface::Border));
```

Start and length are measured in cells, rounded to whole cells, and clipped to the
canvas. See [paint_rule](https://a-n-t-h-o-n-y.github.io/TermOx/rule_8hpp.html)
for arguments and validation.

## Pixel images

`PixelGrid` owns RGB pixels. Each terminal cell displays two vertically adjacent
pixels with a half-block character, so a 20 × 20 image occupies 20 columns and 10 rows:

```cpp
#include <ox/ox.hpp>

int main()
{
    auto image = ox::PixelGrid{ox::Area{20, 20}, ox::RGB{0x182d24}};
    image.set_pixel({5, 5}, ox::RGB{0xffcc88});
    return ox::Application{image}.run();
}
```

Use `set_pixel` to update a pixel and request repaint, or `set_image` to replace
row-major RGB data. Direct edits through `pixels()` or `at()` require a repaint request.
Custom widgets can draw their own RGB storage with `paint_pixels(canvas, size, pixels)`.
See [PixelGrid](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_pixel_grid.html) and the [Explorer image preview](../examples/explorer.md#controls) for loading, zoom,
and panning.

## Scrollable widgets

Implement `Scrollable<T>` from `<ox/scrollbar.hpp>` to use [linked scrollbars](layout.md#link-a-scrollbar):

- `content_size()` and `viewport_size()` return `Area`.
- `scroll_offset()` returns `Point`.
- `set_scroll_offset(Point)` clamps the offset and returns whether it changed.
- `on_scroll(Point offset, Area content, Area viewport)` emits when the offset or extents change.

Units are terminal columns and rows. Keep updates on the application thread and
emit only on changes to avoid feedback loops. The pure functions in
[`<ox/scroll.hpp>`](https://a-n-t-h-o-n-y.github.io/TermOx/scroll_8hpp.html) provide
clamping, reveal calculations, and thumb geometry.

## Headless testing

Application construction, `step()`, and `paint()` need no terminal. Tests can post
input and inspect the resulting frame:

```cpp
auto editor = ox::LineEdit{};
auto app = ox::Application{editor};
app.post(esc::Resize{{20, 1}});
app.post(ox::KeyEvent{.text = U"hello"});
app.step();
app.paint();
// Inspect app.loop().screen.
```

Keep [timers](core-concepts.md#timers) and [animations](appearance.md#motion) as members
when their lifetime should match the widget. For custom text editing, use `TextEdit`,
`TextLayout`, and `<ox/text_view.hpp>`; [text internals](internals/text.md#text-view-composition)
explains their shared caret, hit-test, and painting geometry.
