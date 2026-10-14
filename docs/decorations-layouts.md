# Decorations and container layouts

Border and Divider labels are plain `Label::Options` values, with no Widget lifetime,
focus state, or child enumeration. Border configuration is copyable. The shared
`put_label(Canvas, text, align, style, vertical=false)` draws only the text; it preserves
the line around it. `paint_label` clears the view to styled spaces before drawing.
Both use terminal columns and whole graphemes, validate the complete UTF-8 input, and
respect the canvas width policy and inherited clipping. Labels require positive, known
grapheme widths. Oversized labels keep the longest fitting prefix. A clipped wide
cluster is omitted as a whole, never split.

```cpp
#include <ox/ox.hpp>
using namespace ox;

int main()
{
    auto panel = Column{
        Label{"First"},
        Divider{{.label = {.text = "Section"}}},
        Label{"Second"}} | Border{.label = {.text = "Panel"}};
    arrange(panel, {{}, {30, 10}});
    auto frame = Frame{panel.size()};
    paint_border(Canvas{frame}, panel.border);
    get_child<0>(panel).set_text("Updated");
    panel.child.refresh();
}
```

`border_content_rect(Area)` gives the child's parent-local rectangle. The border uses
one cell on each side; dimensions smaller than two leave an empty interior. Its origin
is clamped to the available area, including zero-sized views. Bordered uses this function
for construction and arrangement. `paint_border(Canvas, Border const&)` draws the box
and overlays the title along the top edge between the corners, leaving the interior
untouched. Label alignment/style are independent of the box style. Label width/height policies
are unused in decorations. Bordered still exposes child/border and supports `widget | Border`.

Divider has explicit `vertical` configuration, defaulting to false, even in a 1×1 view.
Put a vertical divider in a Row and a horizontal one in a Column. Its width is fixed(1) when vertical; its height is fixed(1) when horizontal. Larger views draw the line down the middle of
the cross axis. `paint_divider` clears its view, draws that line, and overlays its label.
Horizontal labels are ordinary text; vertical labels stack one grapheme per row, with
each centered across the width. For vertical labels, Align::Left/Center/Right means
start/middle/end along the vertical axis. A glyph too wide for the view ends the fitting
prefix. Lines::ascii now correctly uses `|` vertically and `-` horizontally.

## Row and Column refresh

Row and Column share one implementation. Keep children in ordinary tuples, pairs,
arrays, vectors, lists, or other sized forward ranges of mutable widget values/references.
Const enumeration and const get_child are supported. There is no replacement container
or ownership adapter; callers own their chosen container.

Edit `layout.children`, child visibility, or SizePolicy normally, then call
`layout.refresh()`. This arranges at the current bounds and schedules application repaint.
`arrange(layout, bounds)` also always refreshes descendants, even when the bounds have
not changed. Refresh the layout whose children changed; a refresh from outside arrangement
can also start at an ancestor to propagate through nested layouts. Public field edits do
not observe themselves automatically.

Arrangement collects visible children without allocating a coroutine. Policies and
private identity slots use reusable vectors. Slots follow moves and clear on destruction
without owning widgets. Membership comparisons avoid copying handles or locking weak
pointers when membership is unchanged. If available size and policies match the
last successful computation, rectangles are reused; every live visible child still gets
its arrangement hook. When geometry/policies change, the existing layout_row/layout_column
functions compute a fresh validated plan before child hooks run. Growth, shrinking, caps,
weights, and deterministic rounding retain the core layout algorithm.

Hooks can relocate children through vector growth/removal, move or destroy the parent,
or request another refresh. A refresh during an active pass is deferred. If membership,
visibility, policies, or size changed, the old plan is discarded and the new plan applied;
redundant unchanged requests settle without recursive arrangement. Callback mutations
also get checked at the end of the pass. Use refresh after removing a still-live child
into another container to discard its old pending arrangement. Mutation callbacks must
settle rather than continually alter their own layout constraints. Changes to nested
containers during a hook should refresh that nested layout directly.

Identity slots are resolved after each callback; no raw child pointer survives into
the next hook. Exceptions reset the arrangement state, allowing a later refresh to recover.
Validation/allocation failures while constructing the plan leave child geometry unchanged.
A throwing child hook can leave an already-applied prefix of child geometry, as with
ordinary arrange. The parent's bounds are updated before its hook is invoked.

Borrowed child enumeration uses one coroutine frame rather than nested forwarding
coroutines for tuple children. Keep the container stable while iterating that borrowed
range. Arrangement itself does not use the borrowed generator.

## Migration

- Border::label and Divider::label change from Label widgets to Label::Options.
  Replace `.label = "Title"` with `.label = {.text = "Title"}`.
- Divider no longer guesses direction from dimensions. Set `.vertical = true` for
  vertical dividers. The previously unused label option now draws its label.
- Row/Column retain their public names, CTAD, ordinary containers, and get_child helpers.
  Use refresh after mutations rather than spelling arrange(layout, layout.bounds()).
- put_label is the drawing-only counterpart of paint_label. paint_label now clears
  previous text when painting directly into a reused canvas.

There are no compatibility adapters.

See [container composition](containers.md) for measurement, gap/cross alignment,
Padding, Stack, ScrollView, and paint layers.
