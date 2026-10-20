# Link scrolling

TextBox, LineEdit, ListView, DataTable, ScrollView, RadioGroup, TreeView, and LogView
share a scrolling interface. Use `link(view, bar)` to connect a `ScrollBar` to either axis.

## Link a view and a scrollbar

```cpp
#include <ox/ox.hpp>

int main()
{
    auto text = ox::TextBox{{.text = "First\nSecond\nThird\nFourth\nFifth\nSixth",
                             .read_only = true}};
    auto vertical = ox::ScrollBar{};
    auto root = ox::Row{std::tie(text, vertical)};
    ox::link(text, vertical);
    return ox::Application{root}.run();
}
```

Resize the terminal to fewer content rows, then scroll with the wheel or drag the
thumb. `link` initializes the bar's range immediately. Geometry, content, and position
changes synchronize both directions, including after a resize without painting first.

Each axis needs its own bar. Set `vertical = false` before linking a horizontal bar;
compose horizontal scrolling below the view with `Column` and `Row`. RadioGroup is
vertical-only: its x offset stays zero and its content width equals its viewport width.

## Connection lifetime

`link(view, bar)` tracks both widget identities, follows moves, and skips expired
targets. The returned pair contains identifiers for `view.on_scroll` and `bar.on_scroll`.
Dropping the pair leaves the connections active. Retain it to disconnect explicitly:

```cpp
auto log = ox::LogView{};
auto bar = ox::ScrollBar{};
auto ids = ox::link(log, bar);
// Later, while both widgets are alive:
log.on_scroll.disconnect(ids.first);
bar.on_scroll.disconnect(ids.second);
```

Connect once per axis. See [Connections and lifetime](connections.md) for callback
ownership and tracked arguments.

## Position and thumb motion

ScrollBar exposes content_length(), viewport_length(), and position() getters,
with public vertical/style configuration.
set_position clamps, schedules paint, and emits on_scroll(int) only on change.
set_range(content, viewport, position) synchronizes without emitting, avoiding feedback
loops. Painting is passive. Wheel steps use content units; clicks center the thumb.
Scrolling always commits immediately and emits `on_scroll` once per changed position.
Set `scroll_settle_time` to a positive duration to animate the thumb toward that
position; animation frames do not change content or emit scroll signals. Nonpositive
durations disable thumb motion. A range change cancels animation; feedback of an
unchanged position/range preserves it. Grabbing a moving thumb stops it at its
displayed position and begins direct dragging.

## Custom scrollable widgets

`Scrollable<T>` in `<ox/scrollbar.hpp>` describes a widget with `content_size()` and
`viewport_size()` returning `Area`, `scroll_offset()` returning `Point`,
`set_scroll_offset(Point)` returning whether it changed, and
`on_scroll(Point offset, Area content, Area viewport)`. Units are terminal columns
and rows. Implement this interface on a custom widget to use the same `link` function.
TreeView and LogView expose this interface directly, so callers need not reach into
their child widgets.

The application thread owns these values and notifications. Emit `on_scroll` only
when the offset or extents change; `set_scroll_offset` must clamp and return whether
its effective offset changed. This keeps reciprocal updates from creating a feedback
loop.

## Scrolling geometry

`<ox/scroll.hpp>` contains pure integer functions, with no widgets, signals, or ownership:

- scroll_limit(content, viewport) and clamp_scroll(position, content, viewport).
- scroll_by(position, delta, content, viewport), with safe signed arithmetic.
- scroll_to_show(position, first, length, content, viewport) to reveal a range.
- scrollbar_thumb(position, content, viewport, track), returning {offset, length}.
- scrollbar_position(cell, content, viewport, track), mapping a centered-thumb click.

Lengths must be nonnegative; positions and deltas can be any int. Empty content occupies
the full scrollbar track. A zero-sized track has no thumb; scrollable content has a thumb
of at least one cell. An oversized reveal range aligns its beginning, an empty range does
nothing, and a zero-sized viewport cannot reveal anything. Products and sums use wider
arithmetic. The same functions drive DataTable, ScrollBar, and vertical text view scrolling.

