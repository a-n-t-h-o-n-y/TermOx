# Frames and canvases

`Frame` owns a grid of terminal cells. `Canvas` borrows a frame and maps local drawing
coordinates onto a clipped rectangle. Their headers contain no terminal session or
event-loop dependency.

```cpp
#include <ox/core/canvas.hpp>
#include <ox/put.hpp>
using namespace ox;

auto frame = Frame{{20, 5}};
auto canvas = Canvas{frame};
auto panel = canvas.subcanvas({2, 1}, {10, 3});
put(panel, {0, 0}, "Hello 界");
fill(panel, bg(colors::blue));
```

## Frame storage

A leading `Cell` owns one complete UTF-32 grapheme, one `Style`, and its number of
terminal columns. The following cells in its footprint have empty text, `columns == 0`,
and the same style. A default cell is a one-column space with the default style.

```cpp
struct Cell {
    std::u32string text = U" ";
    Style style = {};
    std::size_t columns = 1;
};
```

`Frame::write(point, grapheme, style)` accepts exactly one grapheme. Its width comes
from Escape's grapheme and width APIs. Controls, empty input, invalid Unicode, and
zero-column clusters are rejected. Replacing either half of an existing wide glyph
clears its complete old footprint to spaces, retaining the erased cells' styles.
An incoming grapheme is placed only if its entire footprint fits; otherwise the frame
is unchanged and `write()` returns false.

`frame[point]` and `row(y)` provide read-only cells. Both coordinates are checked;
reads outside the frame throw `std::out_of_range`. Use `lead_at(point)` to find the
leading cell of a footprint. Mutation goes through `write()`, `clear()`, and canvas
operations so callers cannot create stray continuation cells.

A frame owns copies of any `esc::WidthPolicy::overrides` supplied to its constructor.
`width_policy()` returns a policy borrowing those owned overrides. Width is a
deterministic estimate under that policy, not a query of the terminal's font.
Policies must assign a space one column, because blank cells have that footprint.
Other graphemes may occupy multiple columns, including more than two with an additive
cluster policy. Copying a frame copies its storage and overrides independently; moving
it leaves the source with size `{0, 0}`.

## Resize and lifetime

Dimensions must be nonnegative. Zero width or height is valid. Size multiplication and
container limits are checked before allocation.

`resize(size)` resets every cell to a default blank, even when the dimensions are
unchanged. It preserves the width policy. Invalid dimensions, impossible sizes, and
allocation failures leave the original frame intact. `clear(style)` blanks the existing
grid without changing its dimensions.

A canvas stores a reference to the frame object, not its cell allocation. Resizing that
frame leaves canvases usable: they retain their original origin, logical size, and
ancestor clip, intersected with the frame's current bounds. Growing the frame can make
previously off-screen portions of a requested view visible. A canvas created for the
whole frame retains the dimensions it had at construction; make a new root canvas to
cover a new size.

The frame must outlive its canvases and stay at the same address. Cell references,
row spans, and borrowed width policies follow their backing storage's lifetime;
resize invalidates cell references and row spans. Moving or assigning a frame can also
invalidate borrowed storage.

## Coordinates and clipping

`Canvas(frame)` covers the frame. `Canvas(frame, origin, size)` requests a rectangle
that may lie partly or entirely outside it. `subcanvas(offset, size)` translates an
offset relative to its parent and inherits the parent's clip. Negative origins and
offsets are supported. A child can never draw outside its ancestor's rectangle.

`size()` is the requested logical size; `origin()` is its frame-space origin.
`visible()` is the visible rectangle in local coordinates, and `contains(point)` checks
that rectangle. `to_global(point)` uses checked coordinate addition. Unrepresentable
coordinates throw `std::overflow_error`. Negative view dimensions are rejected.
Reading through `canvas[point]` outside the visible rectangle throws.

Drawing clips complete graphemes. It never wraps, relocates clipped text, or paints half
of a grapheme. Text beginning at a negative column advances through the omitted
clusters, preserving the positions of the visible remainder.

Overwriting or restyling an existing footprint is also atomic. If that footprint crosses
the clip boundary, the operation leaves it intact and returns false. This prevents a
child canvas from erasing or recoloring cells outside its clip. A full-frame write can
replace either half because the complete old footprint belongs to its view.

## Drawing

`put()` accepts a glyph, UTF-8/UTF-32 text, glyph ranges, and line or box shapes. Text is
validated before any cells are modified. A styled glyph range must assign one style
to every scalar of each grapheme; mixing styles within a single grapheme is rejected.
Shape symbols must occupy one column; a zero symbol in a shape descriptor omits that
part. Line lengths and box dimensions must be nonnegative. Zero extents draw nothing.

Clipping a box preserves its original corners and edges. It does not move them onto
the clip boundary. A one-column box uses its left edge, a one-row box its top edge,
and a one-cell box its top-left corner.

`fill(canvas, glyph)` tiles complete glyph footprints, aligned with local column zero,
and blanks uncovered tail columns. `fill(canvas, character)` preserves existing styles.
Style, foreground, background, and trait fills change appearance without changing text;
they update all cells of each fully visible footprint together. `clear(canvas)` fills
eligible cells with default spaces. Every fill obeys the inherited clip.

The existing widgets now paint through these operations. Text painting measures columns
and avoids splitting graphemes. Their editing APIs still use scalar indices; a complete
redesign of editing and widget behavior belongs to the later widget work.

## Terminal integration

`Cursor` is `std::optional<Point>` in the geometry header. `Terminal` accepts a completed
frame with `commit_changes(frame, cursor)`. Continuation cells are not emitted as text.
A cursor outside the frame is hidden; one on a continuation cell is presented at that
footprint's leading cell. The [renderer](renderer.md) compares frames without terminal
I/O, emits adjacent changes as runs, and retains output storage and presentation state.
