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

## On this page

- [Columns](#columns)
- [Rows and updates](#rows-and-updates)
- [Selection and sorting](#selection-and-sorting)
- [Text and clipping](#text-and-clipping)
- [Keyboard and mouse input](#keyboard-and-mouse-input)
- [Signals](#signals)
- [Scrolling](#scrolling)

## Columns

Each TableColumn has a heading, SizePolicy, alignment, and StylePatch. Width includes
one padding cell on each side. Optional column separators consume one extra cell.
Minimum widths are preserved, allowing horizontal scrolling when they do not fit.
Otherwise `distribute_length` allocates available space using the existing bounds and
weights. Fixed-width columns do not grow. A viewport wider than all maximums leaves
unused background to the right. By default, headers take two rows (text and divider) and body rows take one;
[presentation options](presentation.md#datatable) control separators, padding, and row height. Wrap a table in Bordered when an outer border is wanted.

## Rows and updates

Column and row accessors return borrowed const spans. Use set_data to replace the whole
schema/data, set_rows to replace the rows, insert_row/add_row/erase_row for incremental
updates, and set_cell for individual edits. Data validation rejects malformed UTF-8,
control characters (including tabs and newlines), invalid column policies, incorrect
row lengths, and extents outside the int coordinate range before committing. Invalid
indices throw out_of_range. Configuration/styles remain public; call refresh after
changing show_header, column_lines, selectable, or empty_text. Direct style changes
can instead be followed by an application repaint request. Painting emits no callbacks.

## Selection and sorting

set_data clears selection and both scroll offsets. set_rows clears selection and vertical
scroll. Insert/erase preserves the selected row's identity and the top visible row when
rows before it change; deleting the selected row clears selection. Stable sorting preserves
selection through an index permutation and reveals the selected row. Default sorting is
lexical; supply an ordinary string_view comparator for numeric/domain ordering. It must
provide a strict weak ordering and must not mutate the table. Comparator exceptions leave
data and selection unchanged. Filtering stays with the caller: construct filtered row
vectors and pass them to set_rows. Index values are current view indices, not permanent IDs.

## Text and clipping

Cells reuse TextLayout and put_text_line with the canvas width policy. Combining marks,
wide characters, emoji, and orphan marks keep their complete display footprints. Text
that exceeds a cell is clipped at grapheme boundaries, reserving terminal columns for an
ellipsis. Right/center alignment applies when the entire text fits. Narrow viewports and
inherited clips never draw half a wide glyph. For cell/header patch order and density options, see
[table presentation](presentation.md#datatable).

## Keyboard and mouse input

Arrow Up/Down, Page Up/Down, Home, and End navigate rows and keep selection visible.
An unmodified Enter or Space press/repeat activates the selected row (or selects and
activates the first row). Releases do nothing. Left clicking a body row selects it;
clicking a heading emits on_header(column) without changing selection. Header sorting
is explicit: connect on_header to sort_rows with the desired column-specific ordering.
Left/Right and horizontal wheel events scroll columns; vertical wheel events scroll rows
without changing selection. With selectable=false, navigation scrolls without selection.

## Signals

on_select(optional<size_t>) emits only when the selected index changes; on_activate(size_t)
and on_header(size_t) carry index snapshots. Callbacks may reenter, move, or destroy the
widget. A selection callback that clears selection suppresses pending keyboard activation.
Commands schedule repaint before callbacks. on_scroll(Point, Area, Area) reports offset,
content extent, and viewport extent only when those values change, including on resize
and row-count changes. select and set_scroll_offset return whether visible state changed.
Selecting an invalid row returns false; scrolling clamps to the last full viewport.

## Scrolling

For scrollbar connections, thumb motion, and custom scrollable widgets, use
[Link scrolling](scrolling.md). Table offsets use display columns and rows, including
row spacing and separator cells. The Data page in the
[Showcase](../examples/showcase.md) combines filtering, header sorting, and linked scrolling.
