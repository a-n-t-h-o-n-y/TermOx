# Text editing and layout internals

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

For application editing commands and validation, start with [Text editing](../widgets.md#text-editing).

## On this page

- [Text, carets, and selection](#text-carets-and-selection)
- [Streaming complete lines](#streaming-complete-lines)
- [Navigation and history](#navigation-and-history)
- [Layout and display](#layout-and-display)
- [Storage and caching](#storage-and-caching)
- [Text view composition](#text-view-composition)
- [Line-number caching](#line-number-caching)
- [Paste routing and widget notifications](#paste-routing-and-widget-notifications)

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

## Streaming complete lines

`TextEdit::append_lines(suffix, discard_prefix)` appends LF-terminated records and
removes an optional complete-line prefix, expressed in bytes of the **old** document.
The old document and suffix must be empty or end in LF; a nonzero prefix must end
just after LF. The editor normalizes CR/CRLF in the suffix before checking this rule.
Surviving caret/anchor offsets shift left by the removed bytes; removed endpoints
clamp to zero. This programmatic operation clears undo/redo and pending paste.

`TextLayout::append_lines` has the same byte-prefix contract, requires a prior
`update`, and uses the existing width, wrap, tab width, and width policy. It keeps
retained grapheme widths, wrapped rows, and coordinate mappings and analyzes only the
new suffix. Its suffix is raw layout text, without editor newline normalization.
Both core operations prepare their storage before committing; invalid input or an
allocation failure preserves the original public state.

`TextBox::append_lines` combines these operations, updates any matching measurement
cache, and emits ordinary change/selection/scroll notifications. Like `set_text`, it
is allowed on a read-only control. LogView uses it for bounded streaming. Work still
includes moving retained bytes and indices in contiguous storage; it is not a
constant-time ring buffer.

## Navigation and history

Left/right movement advances one grapheme. Word movement uses spaces, tabs, and line
breaks as separators; it is not linguistic word segmentation. Vertical movement uses
layout columns and retains the desired column through shorter rows. Horizontal moves,
explicit placement, and edits reset that preference. Clicking inside a wide glyph or
tab chooses its leading boundary; clicking past a row chooses its end.

`edit_key(edit, key, layout, validator)` returns `EventResponse`. It supports grapheme
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
the canvas. Drawing walks the layout's cached boundaries and measured widths, without row
resegmentation or per-grapheme glyph vectors. Tabs paint individual spaces, allowing
partial clipping; other graphemes retain complete footprints.

<a id="widget-integration-and-api-changes"></a>

## Storage and caching

TextLayout caches terminal coordinates alongside byte boundaries during construction.
Caret and hit queries use binary searches without decoding or measuring text again.

TextEdit reuses working text and boundary buffers and commits only after preparation
and history allocation succeed. Failed edits, undo, and redo preserve text, boundaries,
selection, and history. Identical-byte selection replacement still records the selection
change but keeps the existing text and boundaries. History uses bounded deques, so
removing the oldest delta does not shift the rest. Working buffers retain their capacity
between edits; the text and boundary vector are still copied to preserve atomicity.

TextLayout separates segmentation and width measurement from wrapping. Width, wrapping,
and tab-stop changes reuse grapheme boundaries and measured columns. Policy changes
remeasure columns. Text changes compare owned snapshots, resegment changed logical lines,
and reuse the unchanged boundary/width prefix and suffix. With unchanged geometry and
policy, wrapping restarts near the changed cluster (including a preceding soft row),
then converges at a surviving row boundary whose remaining graphemes and widths match.
Unchanged rows and coordinates are copied with adjusted byte offsets and row indices.
A shared LF boundary guarantees convergence when Unicode context or wrapping changes
throughout the logical line. Cold layouts and geometry changes still construct all rows.
Tabs are measured at their current row column, and original CR/CRLF offsets are preserved. UTF-8 validation
and control rejection create no discarded decoded or normalized strings. Working text,
metadata, row, coordinate, and override buffers retain capacity between updates.
Failed updates leave the owned snapshot, policy, coordinates, and existing row storage
intact. Cached columns add one integer per grapheme; reusable buffers retain memory
proportional to the largest document. Use the [layout benchmarks](performance.md)
to measure these paths.

## Text view composition

`<ox/text_view.hpp>` provides the functions shared by LineEdit and TextBox. They
operate on a `TextEdit`, `TextLayout`, and `Point` scroll value without requiring a
widget. Scroll uses display columns and rows; carets and selections use UTF-8 byte offsets.

```cpp
#include <ox/text_view.hpp>

auto editor = ox::TextEdit{"hello 界"};
auto layout = ox::TextLayout{};
auto scroll = ox::Point{};
auto const size = ox::Area{20, 3};
auto const wrap = ox::TextWrap::Word;
auto const align = ox::Align::Left;
auto key = ox::KeyEvent{.key = ox::KeyCode::End};
auto response = ox::edit_text(editor, layout, scroll, size, wrap, align, key);

// During painting, update the layout with the actual canvas width policy.
auto frame = ox::Frame{size};
auto canvas = ox::Canvas{frame};
layout.update(editor.text(), size.width, wrap, canvas.width_policy());
scroll = ox::text_scroll(layout, editor.caret(), size, align, scroll);
ox::paint_text(canvas, layout, align, scroll, {}, {.traits = ox::Trait::Inverse},
               editor.selection());
auto point = ox::text_position(layout, editor.caret(), size, align, scroll);
auto cursor = canvas.contains(point) ? ox::Cursor{point} : ox::Cursor{};

// The same aligned viewport maps a mouse hit back to a byte boundary.
editor.set_caret(ox::text_offset(layout, {4, 0}, size, align, scroll));
```

`edit_text` has key and streaming-paste overloads. Both refresh layout and scroll after
an edit; the key overload prepares layout before geometry-dependent bindings. They
use the layout's owned width policy and tab width, return effects, and do not apply
them to an application. An optional scalar predicate filters input; an empty
`std::function` disables it. LineEdit uses this predicate as `accept_character`;
clear it with `accept_character = {}`. See [filtering and submission](../widgets.md#text-editing)
for widget-level validation.

`text_scroll` keeps the caret visible and snaps scrolling to complete grapheme
boundaries. Short right-aligned text shifts left one cell when necessary to display
its end caret. `text_position` and `text_offset` use exactly the same aligned origin
as painting. Alignment applies when a row fits; wider rows use the horizontal viewport
offset. Hit testing clamps extreme coordinates without overflow. `paint_text`
draws only rows visible through the canvas clip and does not clear or fill a background.
The layout's width policy must match the canvas.

`TextLayout::boundaries()` and `widths()` expose borrowed cached analysis. Boundary
spans include the final byte offset; widths contain one entry per cluster, with positive
measured columns, zero for a tab, and minus one for a line break. Tabs expand according
to the current row column and tab stop. These spans have the same mutation/ownership
limits as `lines()`; callers cannot modify the cache.

Widget-specific behavior stays local: LineEdit emits Enter and draws an unfocused,
empty editor's placeholder on its first row; TextBox owns vertical-scroll commands
and coalesced scroll notifications. Both compose the shared input, geometry, and
selection drawing functions. Custom bindings can still use `edit_key` or individual
`TextEdit` operations and call `text_scroll` explicitly.

Optional editor gutters, focus/current-line styling, and composed borders/scrollbars
are described in [editor and table presentation](../appearance.md).

## Line-number caching

TextBox caches logical line starts by document revision. It scans the document only
when text changes and line numbering is enabled, then locates visible numbers with
binary search. Numbering is off by default, so streaming logs avoid that work.
Digit-width changes reflow the text area; offsets stay in document byte coordinates.
See [editor presentation](../appearance.md#editors) for gutter configuration.

## Paste routing and widget notifications

LineEdit and TextBox expose the same content/viewport/offset interface as other
scrollable widgets. Manual scrolling preserves caret and selection; edits reveal
the caret. Painting does not edit the document or emit notifications. `on_scroll`
reports changed offset, content size, or viewport size, including geometry changes.
See [Link scrolling](../layout.md#scrolling) for the shared connection contract.

[Paste routing](routing-and-focus.md#responses-and-propagation) keeps a stream bound
to its original handler. A new paste or text reset discards the retained partial
buffer; a reset cannot cause an orphan final chunk to be inserted.
