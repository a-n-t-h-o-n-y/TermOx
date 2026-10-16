# Data tables and scrolling

DataTable owns rectangular rows of UTF-8 strings and ordinary column descriptions.
It is a read-only view with row selection and activation, not a spreadsheet editor.
Headers remain visible while rows scroll; horizontal scrolling moves headers and cells
together. Only visible cells are analyzed and drawn. It has no child widgets for headings.

```cpp
#include <ox/ox.hpp>
using namespace ox;

int main()
{
    auto table = DataTable{{
        .columns = {{.heading = "Name", .size = SizePolicy::min(16)},
                    {.heading = "Count", .size = SizePolicy::fixed(8),
                     .align = Align::Right}},
        .rows = {{"界 é", "10"}, {"Sample", "2"}}}};
    arrange(table, {{}, {40, 10}});
    table.select(0);
    table.sort_rows(1, true, [](std::string_view a, std::string_view b) {
        return std::stoi(std::string{a}) < std::stoi(std::string{b});
    });
    auto frame = Frame{table.size()};
    table.paint(Canvas{frame});
}
```

Each TableColumn has a heading, SizePolicy, alignment, and StylePatch. Width includes
one padding cell on each side. Optional column separators consume one extra cell.
Minimum widths are preserved, allowing horizontal scrolling when they do not fit.
Otherwise `distribute_length` allocates available space using the existing bounds and
weights. Fixed-width columns do not grow. A viewport wider than all maximums leaves
unused background to the right. Headers take two rows (text and divider); body rows
take one. Wrap a table in Bordered when an outer border is wanted.

Column and row accessors return borrowed const spans. Use set_data to replace the whole
schema/data, set_rows to replace the rows, insert_row/add_row/erase_row for incremental
updates, and set_cell for individual edits. Data validation rejects malformed UTF-8,
control characters (including tabs and newlines), invalid column policies, incorrect
row lengths, and extents outside the int coordinate range before committing. Invalid
indices throw out_of_range. Configuration/styles remain public; call refresh after
changing show_header, column_lines, selectable, or empty_text. Direct style changes
can instead be followed by an application repaint request. Painting emits no callbacks.

set_data clears selection and both scroll offsets. set_rows clears selection and vertical
scroll. Insert/erase preserves the selected row's identity and the top visible row when
rows before it change; deleting the selected row clears selection. Stable sorting preserves
selection through an index permutation and reveals the selected row. Default sorting is
lexical; supply an ordinary string_view comparator for numeric/domain ordering. It must
provide a strict weak ordering and must not mutate the table. Comparator exceptions leave
data and selection unchanged. Filtering stays with the caller: construct filtered row
vectors and pass them to set_rows. Index values are current view indices, not permanent IDs.

Cells reuse TextLayout and put_text_line with the canvas width policy. Combining marks,
wide characters, emoji, and orphan marks keep their complete display footprints. Text
that exceeds a cell is clipped at grapheme boundaries, reserving terminal columns for an
ellipsis. Right/center alignment applies when the entire text fits. Narrow viewports and
inherited clips never draw half a wide glyph. Styles compose table, column, alternating
row, and selected-row appearance; headers apply header_style after the column patch.

Arrow Up/Down, Page Up/Down, Home, and End navigate rows and keep selection visible.
An unmodified Enter or Space press/repeat activates the selected row (or selects and
activates the first row). Releases do nothing. Left clicking a body row selects it;
clicking a heading emits on_header(column) without changing selection. Header sorting
is explicit: connect on_header to sort_rows with the desired column-specific ordering.
Left/Right and horizontal wheel events scroll columns; vertical wheel events scroll rows
without changing selection. With selectable=false, navigation scrolls without selection.

on_select(optional<size_t>) emits only when the selected index changes; on_activate(size_t)
and on_header(size_t) carry index snapshots. Callbacks may reenter, move, or destroy the
widget. A selection callback that clears selection suppresses pending keyboard activation.
Commands schedule repaint before callbacks. on_scroll(Point, Area, Area) reports offset,
content extent, and viewport extent only when those values change, including on resize
and row-count changes. select and set_scroll_offset return whether visible state changed.
Selecting an invalid row returns false; scrolling clamps to the last full viewport.

## Shared scrolling functions

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

ScrollBar exposes content_length(), viewport_length(), and position() getters,
with public vertical/style configuration.
set_position clamps, schedules paint, and emits on_scroll(int) only on change.
set_range(content, viewport, position) synchronizes without emitting, avoiding feedback
loops. Painting is passive. Wheel steps use content units; clicks center the thumb.
Scrolling is immediate by default. Set scroll_settle_time to a positive duration for a
lazy, move-aware timer and elapsed-time easing. Nonpositive durations are immediate.
A range change cancels animation; feedback of an unchanged position/range preserves it.

`link(table, bar)` initializes and synchronizes the selected axis in both directions;
choose bar.vertical before linking. The connection tracks both widgets across moves and
ignores destroyed targets. Link once per axis. TextBox links support either axis; on_scroll(Point, Area, Area) reports
offset, content size, and viewport size. Links return both signal identifiers. Arrange controls through their
parent layout normally; resizing updates the linked ranges even without a paint.

See [the walkthrough](../examples/datatable.md) and its [complete source](../examples/datatable.cpp) for keyboard navigation,
header sorting, numeric ordering, activation feedback, and a linked scrollbar.

## Migration

DataTable's old add_column/headings/column-storage API and offset field are removed.
Initialize columns/rows together or call set_data; use rows(), columns(), selected_row(),
and scroll_offset() to inspect state. The old table on_scroll(int,int) becomes
on_scroll(Point,Area,Area). ScrollBar's scrollable_length/item_visual_length are replaced
by content_length/viewport_length in content units. TextBox::on_scroll(int,int) becomes
on_scroll(Point,Area,Area). There are no compatibility adapters.
