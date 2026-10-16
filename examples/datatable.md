# A sortable table

This program displays 100 rows, links a scrollbar to the table, and updates a
status line when you activate a row. It is a starting point for an inventory
browser or a list of search results. The data here is generated in memory.

[Complete source: datatable.cpp](datatable.cpp)

![Table example after selecting the first row](../docs/img/table-v3.svg)

The image is rendered from the example's frame at 88 columns by 14 rows, after
pressing Down. Terminal colors and fonts may differ.

## Run it

From the repository root, after [configuring the build](../docs/getting_started.md):

```sh
cmake --build build --target TermOx.examples.datatable
./build/examples/TermOx.examples.datatable
```

- Press Tab to focus the table if needed.
- Use Up/Down, Page Up/Down, Home, and End to select rows.
- Press Enter or Space to activate a row. Its name appears in the status line.
- Click a column heading to sort; click again to reverse the order.
- Scroll with the mouse wheel or scrollbar. Left/Right scrolls horizontally.
- Press Ctrl-C to exit.

Activation only changes the status line in this example; it does not open a file
or another view.

## Lay out the controls

The outer `Column` holds a `Row` and a one-line `Label`. The row holds the table
and its vertical scrollbar. The table takes the available space above the status
line. `get_child<N>()` returns a reference to a child owned by a layout:

```cpp
auto& body = get_child<0>(root);
auto& table = get_child<0>(body);
auto& bar = get_child<1>(body);
auto& status = get_child<1>(root);
```

Name and Description have minimum widths, so they can grow. Count has a fixed
width and right-aligned text. When the columns no longer fit, the table scrolls
horizontally instead of shrinking them below their minimums.

`DataTable::measure()` reports the full data height. For a long list, that can
crowd out the status line. This example derives `ResultTable` and caps its
preferred height at three rows: two for the header and one for data. Its height
policy remains flexible, so the layout gives it the remaining space. This changes
the preferred size, not the number of rows that can be displayed.

## Supply rows and link the scrollbar

Rows are vectors of UTF-8 strings. Every row must have one cell per column.
`set_rows()` transfers the generated rows into the table and clears its selection
and vertical scroll position.

```cpp
table.set_rows(std::move(rows));
link(table, bar);
```

`link()` synchronizes the scroll range and position in both directions, including
when the table is resized. It tracks both widgets across moves. Link once per
axis; there is no need to copy scrollbar positions in the event callbacks.

The descriptions are UTF-8 strings. Text that exceeds a column is clipped at
grapheme boundaries, with an ellipsis where space permits.

## Sort when a heading is clicked

A heading click emits `on_header(column)`. The table leaves the sorting policy to
the application. The example remembers a direction for each column and toggles
it on each click:

```cpp
auto ascending = std::array{false, false, false};
table.on_header.connect([&](std::size_t column) {
    ascending[column] = !ascending[column];
    // Call sort_rows with the chosen direction and comparator.
});
```

Name and Description use the default lexical comparison. Count uses this numeric
comparison so that `2` comes before `10`:

```cpp
[](std::string_view a, std::string_view b) {
    return std::stoi(std::string{a}) < std::stoi(std::string{b});
}
```

The generated counts are valid integers. For imported data, validate values before
putting them in the table, or provide a comparator that handles invalid cells.
The comparator must not modify the table.

Sorting preserves the selected row's identity, but its index can change. A row
index is a position in the current view, not a permanent record ID. An application
that edits or opens records should keep its own IDs.

## Respond to activation

The activation signal supplies the current row index:

```cpp
table.on_activate.connect([&](std::size_t row) {
    status.text = "Opened " + table.rows()[row][0];
    app.request_repaint();
});
```

This callback writes directly to `Label::text`, then requests a repaint. For a
label whose text affects layout, prefer `status.set_text(...)`, which requests
layout as well as painting.

The callbacks capture local references. This works here because the widgets and
sort state stay in place and remain alive throughout `app.run()`. If you extract
this setup into a function that returns or moves a widget tree, use
[`Connection` with tracked widget arguments](../docs/getting_started.md#read-the-counter)
and give the sort state an owner that outlives the callbacks.

## Extend it

To filter results, build a new row vector and pass it to `set_rows()`. To update
one cell, use `set_cell()`. Keep domain data outside the table when you need typed
values, persistent IDs, or saving to disk.

See [Data tables and scrolling](../docs/data-tables.md) for the full behavior of
selection, sorting, clipping, and scroll signals.
