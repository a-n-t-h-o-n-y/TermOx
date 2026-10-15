# Quiet details for editors and tables

Build `TermOx.examples.polish` and run it to compare Minimal, Refined and Roomy
presentations. Its switches enable details independently; its theme selector compares
dark, light and terminal colors. Alt-S stacks the panes. The example composes existing
`Padding`, `Border`, `SplitPane`, `ScrollBar` and `link` with ordinary TextBox/DataTable
controls. `--check` exercises three presets, three themes and five terminal sizes.

The visual roles are inspired by the independently styled header/cell/selection and
editor text/gutter/current-line roles in [Bubbles](https://github.com/charmbracelet/bubbles).
TermOx keeps its existing owned state, direct options and Canvas painting.

## TextBox and LineEdit

The shared editor options/public fields add these style patches:

| Field | Role |
| --- | --- |
| `focused_style` | Applied to the control's base style while focused |
| `blurred_style` | Applied while unfocused |
| `inactive_selection_style` | Applied after selection styling while unfocused |
| `gutter_style` | Line-number gutter colors and traits |
| `current_line_style` | Current display row, when highlighting is enabled and focused |

`line_numbers` (default `false`) enables a fixed gutter on TextBox. Numbers describe **logical**
lines, starting at one; wrapped continuation rows display `↪` (`>` in the terminal
ASCII theme). The gutter includes a one-cell gap and grows with the number of digits.
It does not scroll horizontally. Clicking it selects the complete logical line.
`viewport_size()`/`on_scroll` report the text area, excluding this gutter; mouse hits,
selection, caret visibility, keyboard navigation and wrapping use that same area.
LineEdit ignores `line_numbers`.

`highlight_line` (default `false`) enables a restrained highlight on the focused display row.
The themed default mixes the control background toward the selection color; its
explicit patch applies afterwards. The highlighted row retains ordinary text-selection
colors. The current logical line number uses the theme accent while focused.

Styled themes use a quieter foreground for unfocused selections. Explicit selection
colors retain precedence; `inactive_selection_style` can override them for the
unfocused state. Plain themes continue to use explicit patches and inverse selection.
LineEdit's accent/underline focus treatment respects an explicit focus foreground.

```cpp
auto editor = ox::TextBox{{.text = "Notes\n",
                            .line_numbers = true,
                            .highlight_line = true}};
// Compose outer spacing and borders using Padding/Border, and scrollbars using link.
```

Presentation fields are ordinary mutable values: call `refresh()` after changing
them. Numbering caches line starts by text revision; it scans the document only when
text changes and numbering is enabled, then locates visible numbers with binary
search. Numbering is off by default, so streaming logs keep their original path.
Digit-width changes necessarily reflow wrapped text. Both selection and caret offsets
remain UTF-8 byte boundaries, independent of the gutter.

## DataTable

Column widths continue to include their padding. `cell_padding=1` controls horizontal
padding per side; narrow columns clamp their text area to zero safely. `row_height=1`
controls body height in terminal rows, with text vertically centered. It must be
positive; padding must be nonnegative. Headers remain one row tall. `header_separator=true`
adds a rule below them; `show_header=false` removes both. `row_lines` (default `false`) adds rules
between body rows, independently of the existing `column_lines` switch.

Scroll offsets and extents remain **display cells**, including row spacing/rules.
Mouse selection and hover ignore separator rows; keyboard pages and wheel navigation
account for row height. Selecting a row reveals it as a complete row when it fits.
Model mutations validate coordinate limits before committing. Extremely large density
settings that overflow terminal coordinates are rejected.

| Field | Role |
| --- | --- |
| `striped=false` | Subtle themed alternating-row background; `alternate_style` overrides it |
| `hover_rows=false` | Pointer row highlighting; `hover_style` overrides its themed background |
| `focused_style`, `blurred_style` | Base appearance of focused/unfocused controls |
| `inactive_selected_style` | Additional patch for the unfocused selection |
| `empty_style` | Empty-state appearance, based on the muted theme role |
| `TableColumn::heading_style` | Header appearance independent of the existing body-column `style` |
| `cell_style(row,column)` | Optional pure function returning an additional `StylePatch` |
| `show_sort_indicator=true` | Show a small ascending/descending marker after explicit sorting |

Headers use the muted theme role with the existing bold header patch. Selection uses
the accent when focused and a quieter foreground when unfocused. Explicit colors win.
Hover applies below selection, so moving the pointer never obscures the selected row.
The cell style order is base/focus → column → stripe/alternate → callback → hover →
selection → inactive selection. The header order is muted base/focus → header
patch → column heading patch; sort markers use the theme accent.

`cell_style` runs only for visible cell rectangles. It may read immutable application
state or the table's const data accessors; it must not mutate, move or destroy the
control. It returns a patch, not formatted text or terminal escapes. For example,
return a muted foreground for numeric metadata or semantic positive/negative colors
for status values. Sorting and selection continue to operate on the original strings.

`sort_order()` returns optional `{column, ascending}` after `sort_rows`. The selected
row remains selected through sorting. Replacing data, inserting a row or modifying
the sorted column clears the indicator; erasing rows or changing another column
preserves it. Comparators that throw preserve the previous model/order. Header clicks
still emit `on_header`; the application chooses whether/how to sort. Unicode markers
become `^`/`v` in the ASCII theme. The example demonstrates numeric sorting explicitly.

```cpp
auto table = ox::DataTable{{.columns = {{"Project"}, {"Status"}},
                            .rows = {{"Atlas", "Ready"}},
                            .striped = true,
                            .hover_rows = true,
                            .header_separator = true,
                            .row_lines = false,
                            .cell_padding = 2,
                            .row_height = 3}};
```

Call `refresh()` after changing presentation fields. Use mouse move reporting for
hover (`Application::run({.mouse_mode = MouseMode::Move})`). Plain themes use explicit
stripe/hover patches; styled themes supply the subtle automatic colors. Border shape,
outer spacing, scrollbars, captions and status/help lines remain ordinary composition.

```sh
cmake --build build-release --target TermOx.examples.polish
./build-release/examples/TermOx.examples.polish
./build-release/examples/TermOx.examples.polish --check
python3 tests/smoke_polish.py build-release
```
