# Text editing core

`<ox/core/text.hpp>` provides owned UTF-8 editing state and cached text layout without
widgets, frames, event loops, or terminal I/O. It uses Escape's Unicode conversion,
grapheme segmentation, and width policies.

```cpp
#include <ox/core/text.hpp>

auto edit = ox::TextEdit{"é界👩‍💻"};
edit.set_caret(edit.text().size());
edit.erase_backward(); // Removes the complete emoji sequence.
edit.select_all();
edit.insert("new text");
edit.undo(); // Restores the previous text, selection direction, and caret.

auto layout = ox::TextLayout{};
layout.update(edit.text(), 12, ox::TextWrap::Word);
auto caret = layout.position(edit.caret());
auto clicked = layout.offset({3, 0});
edit.set_caret(clicked);
```

## Text, carets, and selection

`TextEdit` owns its text. `text()` returns a const string reference. Caret and anchor
are **UTF-8 byte offsets**, constrained to complete grapheme boundaries. They are
neither Unicode scalar indices nor terminal columns. `boundaries()` exposes the
cached boundaries; `set_caret(offset, extend)` rejects an interior or out-of-range
offset. `selection()` returns the ordered half-open byte range. Extending preserves
the anchor; ordinary left/right movement collapses an existing selection toward its
corresponding edge. `select_all()` selects the complete document.

Insertions replace selections. Backspace/Delete remove complete graphemes or the
selection. Mutations resegment the affected logical lines: inserting a combining mark
or regional indicator can join neighboring clusters. Segmentation starts after the
previous LF and continues through the next surviving LF after the replaced range;
deleting newlines includes all joined lines. LF resets grapheme context, so unchanged
lines retain their cached boundaries. A single long line is resegmented in full. The resulting caret snaps to
the next complete boundary after the inserted bytes. Empty or fully rejected insertions
preserve a selection; use deletion explicitly to remove it.

Both modes normalize CR and CRLF to LF and reject other terminal control characters.
Multiline mode preserves LF and tabs. Single-line mode, selected with
`TextEdit(text, false)`, removes line breaks and tabs. Optional insertion validators
filter Unicode scalars after validation and normalization. Validators should be pure;
exceptions propagate without committing the proposed edit.

`set_text()` replaces the document, resets caret and anchor to zero, and clears history
and pending paste. Text and layout objects own their storage and copy independently.
Moved-from objects remain usable empty objects. Returned string references, boundary
spans, and layout row spans must not be retained across mutation, assignment, or moves.

## Navigation and history

Left/right movement advances one grapheme. Word movement uses spaces, tabs, and line
breaks as separators; it is not linguistic word segmentation. Vertical movement uses
layout columns and retains the desired column through shorter rows. Horizontal moves,
explicit placement, and edits reset that preference. Clicking inside a wide glyph or
tab chooses its leading boundary; clicking past a row chooses its end.

`key_press(key, layout, validator)` returns `EventResponse`. It supports grapheme
arrows, Ctrl+Left/Right and Ctrl+Backspace/Delete, Shift selection, Home/End on the
display row, Ctrl+Home/End on the document, vertical arrows, Ctrl+A, Ctrl+Z,
Ctrl+Y/Ctrl+Shift+Z, printable key text, and multiline Enter. Releases and unused keys
are unhandled. A recognized command requests repaint only when editing state changes.
Refresh the layout after text or geometry changes; geometry-dependent commands reject
a stale text snapshot.

Undo/redo record edit deltas and selection state, rather than full-document snapshots.
The constructor's third argument bounds the number of retained changes (default 100;
zero disables history). Each insertion, deletion, or completed paste is one edit.
Caret movement does not create history. A new edit clears redo; resetting text clears
both histories. Validation and allocation failures leave the edit uncommitted.

`paste(chunk, validator)` buffers bytes until the final chunk, allowing UTF-8 scalars
to span chunks. Successful completion inserts once and creates one history entry.
Aborted streams are discarded. Orphan continuations, including those following a text
reset, are ignored. Invalid final UTF-8 or control characters throw without changing
the document or history. Paste normalization uses the editor's line mode.

## Layout and display

`layout_text(text, width, wrap, policy, tab_width)` is a pure function returning owned
`TextLine` values. `begin`/`end` delimit displayed text; `next` also consumes an explicit
line break. `columns` measures terminal cells. `TextLayout::update()` caches those rows,
its own text snapshot, grapheme boundaries, and a copy of width-policy overrides.
Unchanged updates retain storage and skip decoding and layout calculation.

`TextWrap::None` preserves logical lines. `Any` wraps at grapheme boundaries. `Word`
prefers the last space or tab that fits and otherwise breaks at a grapheme boundary.
Whitespace is preserved. A glyph wider than the available width gets its own row and
is clipped as a complete footprint when drawn. Width zero retains logical lines.
Negative width, invalid policy/wrap values, and nonpositive tab stops throw.

`position(offset)` maps bytes to columns and rows; `offset(point)` maps a display hit
back to a boundary. A soft-wrap boundary belongs to the following row. A final full
wrapped row gets an empty caret row, making the end-of-document caret representable.
Explicit trailing line breaks also retain an empty final row. This row counts toward
scroll length.

Tabs advance to the next stop (four columns by default), measured from the display
row's beginning. `display_grapheme()` expands them to spaces and adds a dotted circle
to zero-width standalone clusters when possible. Layout measures that same display
form under its supplied width policy. Space must be one column, matching `Frame`'s
contract; unknown or unrenderable widths are rejected. Column and row counts are
checked before converting to integer geometry.

`put_text_line()` composes cached layout with a canvas, normal and selected styles,
and a selection range. It expands display glyphs, paints whole footprints, and marks
a selected newline with a space when visible. The layout's width policy must match
the canvas. LineEdit and TextBox use this same function for painting.

## Widget integration and API changes

Both text widgets expose `TextEdit editor`, `text()`, and `set_text(value)`. The previous
mutable `text` string field and scalar-index caches have been removed. Options still
accept an initial UTF-8 string. Direct setters request repaint when attached to an
application; direct edits through `editor` should be followed by an application repaint
request or an event response that requests repaint.

LineEdit uses single-line mode, cell-based horizontal scrolling, Left/Center/Right
alignment, and Shift-click selection. It keeps the caret visible without splitting
graphemes at the viewport edge. Enter emits a stable string snapshot, so slots can
replace or clear the editor safely.

TextBox uses `TextWrap` (replacing `TextBox::Wrap`), cached multiline layout, vertical
navigation, alignment-aware hit testing, selection painting, and clamped scrolling.
`set_scroll_offset()` returns whether visible editing state changed. Scroll signals
are emitted only when the top row or row count changes. Both widgets expose a
`selection_style` patch, defaulting to inverse video.

Applications bind a streaming paste to the widget that handled its first chunk.
Changing focus does not redirect its remaining chunks. Removal or disabling drops
the remaining stream; a new paste or text reset discards any retained partial buffer.
A reset during paste never inserts an orphan final chunk.

Headless tests cover normalization and rejection, combining sequences and emoji,
selection and grapheme joining, bounded history, paste transactions, wrapping and
mapping, width-policy ownership, vertical navigation, cache reuse, extreme hit
coordinates, randomized edits, and widget/application integration.

TextLayout caches terminal coordinates alongside byte boundaries during construction.
Caret and hit queries use binary searches without decoding or measuring text again.
See [performance benchmarks](performance.md) for timings and the storage tradeoff.

TextEdit reuses working text and boundary buffers and commits only after preparation
and history allocation succeed. Failed edits, undo, and redo preserve text, boundaries,
selection, and history. Identical-byte selection replacement still records the selection
change but keeps the existing text and boundaries. History uses bounded deques, so
removing the oldest delta does not shift the rest. Working buffers retain their capacity
between edits; the text and boundary vector are still copied to preserve atomicity.

TextLayout separates segmentation and width measurement from wrapping. Width, wrapping,
and tab-stop changes reuse grapheme boundaries and measured columns. Policy changes
remeasure columns; text changes redo segmentation and measurement. Tabs are measured
at their current row column, and original CR/CRLF offsets are preserved. UTF-8 validation
and control rejection create no discarded decoded or normalized strings. Working text,
metadata, row, coordinate, and override buffers retain capacity between updates.
Failed updates leave the owned snapshot, policy, coordinates, and existing row storage
intact. Cached columns add one integer per grapheme; reusable buffers retain memory
proportional to the largest document. See the layout construction measurements in the
[performance guide](performance.md).
