# Drawing, layout, and connection conventions

Drawing functions borrow a `Canvas`; they neither own widgets nor perform terminal
I/O. Widgets compose those functions and hold the state that drives them. Canvas
coordinates are parent-local, and its inherited clip controls visible writes.

`put(canvas, at, glyphs, patch)` accepts spans, contiguous containers, and other glyph
ranges with identical StylePatch behavior. Text remains grouped into complete Unicode
graphemes; each grapheme requires one source style. UTF-8/text/style validation precedes
writes. Shape symbols must occupy one column. `validate(Style const&)` shares color
channel validation between drawing helpers, Canvas, and Frame. Allocation failures
while painting are not a frame transaction.

`fill_outside(canvas, content, glyph)` fills margins around a parent-local Rect, after
intersecting the content with the logical canvas. Empty or disjoint content fills the
whole visible view. Full content leaves it untouched. Negative dimensions or an invalid
fill glyph are rejected before drawing, even if there is no margin to paint. Complete
wide content glyphs and inherited clips remain protected by Canvas.

```cpp
#include <ox/layout.hpp>

auto panel = ox::Suspended{ox::Widget{}, ox::Area{12, 4}};
ox::arrange(panel, {{}, {20, 10}});
panel.requested_size = {16, 6};
panel.refresh();
```

Suspended owns its child. Its refresh recomputes centered, clamped geometry and notifies
the child even when geometry is unchanged, matching Row and Column. Painting composes
fill_outside; normal application traversal paints the child. Invalid requested sizes
leave child geometry unchanged. Direct fill_glyph changes need a repaint request.

```cpp
#include <ox/widget.hpp>

sl::Signal<void(int)> changed;
ox::Widget target;
auto id = ox::connect(changed, [](int value, ox::Widget& widget) {
    widget.enabled = value != 0;
}, target);
changed(1);
changed.disconnect(id);
```

The signal owns the callback and its captures. connect returns the existing
SignalsLight identifier for explicit disconnection; it has no destructor behavior.
Dropping the identifier keeps the slot connected. A disconnected slot returned by
signal.disconnect can still be called and keeps its captures alive while retained.
Callbacks must be copyable, as required by the signal's std::function storage.

Tracked widgets are borrowed. Their references follow moves, and callbacks are skipped
if any target expires or loses its expected type through slicing. Const targets work.
Empty callbacks are rejected before wrapping/subscribing. Calling tracked directly
composes the same borrowing rules without connecting a signal. Signal invocation and
widget mutation belong to the application thread. Callback-owned captures survive
expiration of a tracked target until the slot is removed or its signal is destroyed.
Pass widgets as tracked arguments rather than capturing references to their locations.

Migration:

| Previous API | Current API |
| --- | --- |
| `Connection{.signal = signal, .slot = fn}(widgets...)` | `connect(signal, fn, widgets...)` |
| `find_align_offset(align, available, used)` | `align_offset(align, available, used)` |
| Re-arrange Suspended to apply requested_size | `panel.refresh()` |
| Manual margin subcanvas construction | `fill_outside(canvas, content, glyph)` |
| Unused VAlign enum | Removed; centered geometry uses centered_rect |
| Unused InputRangeOf concept/common.hpp | Standard range concepts; header removed |

Alignment counts terminal columns and requires nonnegative extents; oversized content
can produce a negative offset. Invalid enum values are rejected. WidgetDerived requires
public inheritance and accepts const-qualified targets for borrowing. Widget's header
includes the drawing, events, geometry, and timer declarations it uses directly, rather
than relying on the core umbrella header to expose unrelated terminal machinery.

Validation: 221 Release cases (including eight allocation-fault cases), all seven
example builds, nine PTY smoke checks, independent compilation of all 34 public
headers, and compilation of the README/guide examples pass. ASan+UBSan passes 122
non-exception cases with every TermOx source instrumented and the unchanged Escape
Release library linked. Deliberate exception tests run in Release because the installed
sanitizer runtime cannot unwind C++ exceptions.
