# Activation and row selection

[Controls](../controls.md) and [selection widgets](../selection-and-application.md)
cover widget use.

`<ox/controls.hpp>` supplies activation and index-selection functions without widgets,
item ownership, signals, or event-loop state. Button, CheckBox, and RadioGroup use the
same functions; custom controls can use them too.

```cpp
#include <ox/controls.hpp>

auto selected = std::optional<std::size_t>{};
auto response = ox::move_selection(selected, 5, 1); // Highlight the first item.
response = ox::select_index(selected, 3, 5);        // Select index three.
auto key = ox::KeyEvent{.key = U' '};
if (ox::activates(key)) { /* Activate the highlighted item. */ }
```

## Activation and selection helpers

`activates` accepts an unmodified Enter/Space press or a left mouse press.
Key repeats and releases do not activate. `releases` recognizes activation-key
releases and left/unspecified mouse releases; widgets match these to their own gestures.

`select_index(selected, index, count)` rejects out-of-range values, returning no effects.
A valid value is handled and requests repaint only if it differs. `move_selection`
clamps navigation, initializes an empty optional to zero, and safely normalizes stale
indices after item removal. Empty lists clear the optional and remain unhandled.
Both return EventResponse and never schedule painting or emit callbacks.

## Shared row components

`<ox/selection.hpp>` supplies the shared row model and helpers.

`RowSelection` contains sorted selected `rows`, a keyboard
`current` row, and a range `anchor`. `select_rows` composes replace, extend, toggle,
and cursor-only actions; an optional predicate excludes disabled rows. `row_navigation`
interprets arrows, page keys, Home, and End, while `row_target` finds an eligible row.
`visible_rows` calculates a clipped half-open interval. `paint_row_text` shares Unicode
alignment, whole-grapheme clipping, ellipsis, and caller-owned TextLayout storage.
DataTable uses the same navigation and visible-row/text painting functions.

For custom event handlers, see [Writing a custom widget](../widget-api.md).
