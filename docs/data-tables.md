# Display and sort a table

DataTable owns rectangular rows of UTF-8 text. Cells are read-only; users can select
or activate rows. Headers stay visible while rows scroll. This example sorts the
Count column numerically:

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
    table.select(0);
    table.sort_rows(1, true, [](std::string_view a, std::string_view b) {
        return std::stoi(std::string{a}) < std::stoi(std::string{b});
    });
    return Application{table}.run();
}
```

## Columns

Configure each column's heading, size policy, alignment, and style. Minimum widths
are preserved, allowing horizontal scrolling when columns do not fit. Fixed-width
columns do not grow. See [table presentation](presentation.md#datatable) for
padding, separators, row height, and style precedence. Use `Border` for an outline.

## Rows and updates

Column and row accessors return borrowed const spans. Use `set_data` to replace the
schema and data, `set_rows` to replace rows, or `set_cell` for an individual edit.
Use `insert_row`, `add_row`, and `erase_row` for incremental updates.
Validation rejects malformed UTF-8,
control characters (including tabs and newlines), invalid column policies, incorrect
row lengths, and extents outside the int coordinate range before committing. Invalid
indices throw `std::out_of_range`. Call `refresh()` after
changing `show_header`, `column_lines`, `selectable`, or `empty_text`. Direct style changes
can instead be followed by an application repaint request. Painting emits no callbacks.

## Selection and sorting

`set_data` clears selection and both scroll offsets. `set_rows` clears selection and
vertical scroll. Insert/erase preserves the selected row's identity and the top visible row when
rows before it change; deleting the selected row clears selection. Stable sorting preserves
selection through an index permutation and reveals the selected row. Default sorting is
lexical; supply a `string_view` comparator for numeric or domain ordering. It must
provide a strict weak ordering and must not mutate the table. Comparator exceptions leave
data and selection unchanged. Filtering stays with the caller: construct filtered row
vectors and pass them to `set_rows`. Index values are current view indices, not permanent IDs.

## Text and clipping

Text that exceeds a cell is clipped at grapheme boundaries with an ellipsis.
Combining sequences and emoji stay together; clipped views never draw half a wide
glyph. Right/center alignment applies when the entire text fits.

## Keyboard and mouse input

Arrow Up/Down, Page Up/Down, Home, and End navigate rows and keep selection visible.
An unmodified Enter or Space press/repeat activates the selected row (or selects and
activates the first row). Releases do nothing. Left clicking a body row selects it;
clicking a heading emits `on_header(column)` without changing selection. Header sorting
is explicit: connect `on_header` to `sort_rows` with the desired column-specific ordering.
Left/Right and horizontal wheel events scroll columns; vertical wheel events scroll rows
without changing selection. With `selectable = false`, navigation scrolls without selection.

## Signals

`on_select` emits only when the selected index changes; `on_activate`
and `on_header` carry index snapshots. Callbacks may reenter, move, or destroy the
widget. A selection callback that clears selection suppresses pending keyboard activation.
Commands schedule repaint before callbacks. `on_scroll` reports offset,
content extent, and viewport extent only when those values change, including on resize
and row-count changes. `select` and `set_scroll_offset` report whether visible state changed.
Selecting an invalid row returns false; scrolling clamps to the last full viewport.

## Scrolling

For scrollbar connections, thumb motion, and custom scrollable widgets, use
[Link scrolling](scrolling.md). Table offsets use display columns and rows, including
row spacing and separator cells. The Data page in the
[Showcase](../examples/showcase.md) combines filtering, header sorting, and linked scrolling.
