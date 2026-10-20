# Widget presentation

Widget options control colors, selection styles, spacing, and indicators.

## On this page

- [TextBox and LineEdit](#textbox-and-lineedit)
- [DataTable](#datatable)
- [Lists and trees](#lists-and-trees)
- [Buttons and choice indicators](#buttons-and-choice-indicators)
- [Values, activity and tabs](#values-activity-and-tabs)

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

`highlight_line` (default `false`) highlights the focused display row.
The themed default mixes the control background toward the selection color; its
explicit patch applies afterwards. The highlighted row retains text-selection
colors. The current logical line number uses the theme accent while focused.

Styled themes use the muted foreground for unfocused selections. Explicit selection
colors retain precedence; `inactive_selection_style` can override them for the
unfocused state. Plain themes use explicit patches and inverse selection.
LineEdit's accent/underline focus treatment respects an explicit focus foreground.

```cpp
auto editor = ox::TextBox{{.text = "Notes\n",
                            .line_numbers = true,
                            .highlight_line = true}};
// Compose outer spacing and borders using Padding/Border, and scrollbars using link.
```

Call `refresh()` after changing presentation fields. Gutter width changes reflow
wrapped text; caret and selection offsets remain UTF-8 byte boundaries. The
[line-number cache](internals/text.md#line-number-caching) is described under internals.

## DataTable

Column widths include their padding. `cell_padding=1` controls horizontal
padding per side; narrow columns clamp their text area to zero safely. `row_height=1`
controls body height in terminal rows, with text vertically centered. It must be
positive; padding must be nonnegative. Headers remain one row tall. `header_separator=true`
adds a rule below them; `show_header=false` removes both. `row_lines` (default `false`) adds rules
between body rows, independently of `column_lines`.

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
| `TableColumn::heading_style` | Header appearance independent of the body-column `style` |
| `cell_style(row,column)` | Optional pure function returning an additional `StylePatch` |
| `show_sort_indicator=true` | Show a small ascending/descending marker after explicit sorting |

Headers use the muted theme role with the bold header patch. Selection uses
the accent when focused and the muted foreground when unfocused. Explicit colors win.
Hover applies below selection, so moving the pointer never obscures the selected row.
The cell style order is base/focus → column → stripe/alternate → callback → hover →
selection → inactive selection. The header order is muted base/focus → header
patch → column heading patch; sort markers use the theme accent.

`cell_style` runs only for visible cell rectangles. It may read immutable application
state or the table's const data accessors; it must not mutate, move or destroy the
control. It returns a patch, not formatted text or terminal escapes. For example,
return a muted foreground for numeric metadata or semantic positive/negative colors
for status values. Sorting and selection operate on the original strings.

Sort markers use `^`/`v` in the ASCII theme. Header clicks emit `on_header`; the
application chooses whether and how to sort. See [table selection and sorting](data-tables.md#selection-and-sorting)
for selection preservation, sort state, and custom comparators.

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
stripe/hover patches; styled themes supply default colors.
Use separate widgets for borders, spacing, scrollbars, captions, and status/help lines.

## Lists and trees

`ListView` shares DataTable's row styles. `striped` and
`hover_rows` default to `false`. `alternate_style` applies to odd rows regardless
of `striped`; the flag enables an automatic themed background. `hover_style` applies
only when row hover is enabled and the item is enabled. Hover follows the pointer's
viewport position when scrolling and clears on pointer exit or model replacement.
Use mouse move reporting to enable it.

Style order is base/focus (`focused_style` or `blurred_style`) → `ListItem::style` →
stripe/`alternate_style` → hover → selection → `inactive_selected_style` when unfocused
→ focused current row → disabled. Selection wins over hover. Inactive selection uses
the theme's muted foreground by default, preserving explicit selection patches.
`indicator_style` controls the leading marker separately. `show_indicator=false`
removes its two-cell gutter; measurement, horizontal scrolling and text placement
use the same width. Plain themes retain their undecorated list presentation.

`empty_text` displays one row of help when there are no items; `empty_style` patches
its muted appearance. It contributes one natural row for intrinsic layouts, but creates
no selectable item or scroll extent. Call `refresh()` after changing presentation fields.

```cpp
auto choices = ox::ListView{{.items = {{"Inbox"}, {"Review"}, {"Done"}},
                             .multiple = true,
                             .striped = true,
                             .hover_rows = true,
                             .empty_text = "No matches"}};
```

Menus inherit these fields, and Select/ComboBox expose their lists as `popup.child`.
Configure those components directly. CommandPalette and FilePicker also share the
ListView renderer and its inactive-selection styles. LogView and
numeric editors use the TextBox/LineEdit options on `editor`.

TreeView accepts shared row presentation in `Options::list`. Each `TreeNode::style`
styles its row, including after expansion rebuilds. `show_guides` (default `true`)
controls connector lines independently of expand/collapse markers, and `guide_style`
patches their subdued border color. Guide placement respects the list's marker gutter
and horizontal scrolling. Terminal themes use ASCII connectors. Paths, keyboard
navigation and activation do not depend on guide visibility.

## Buttons and choice indicators

Button adds `hover_style`, `blurred_style` and `show_focus_indicator` (default `true`).
The style order is themed feedback → focus/blur → hover → pressed → disabled.
`show_focus_indicator=false` hides the keyboard focus marks without changing focus
styles or button spacing.

Checkbox and switch labels keep the normal text color; accent/muted colors belong
to their indicators. `indicator_style` patches either state, then `checked_style` or
`unchecked_style` applies to the indicator alone. RadioGroup uses the same split, with
`indicator_style`, `selected_style` and `unselected_style`. `focused_style` still
applies to the focused label and indicator, so focus does not rely on color alone.
Disabled styling applies last. Glyphs with explicit colors retain them unless an
explicit indicator/state/focus patch overrides them. Label alignment and indicator
width measurement still use display cells, including for wide Unicode glyphs.

Glyph default styles and explicit patches compose as described in
[Custom drawing](drawing.md#styles-and-glyphs).

## Values, activity and tabs

Slider exposes `track_style`, `fill_style`, `thumb_style` and `focused_style`.
`show_fill=false` leaves a uniform track; `show_thumb=false` hides the thumb. Both
switches default to `true`. These controls affect presentation only: pointer capture,
value mapping and keyboard steps work identically in either orientation. The thumb
inherits the fill style before its own patch. ScrollBar similarly exposes
`track_style` and `thumb_style`, without changing its linked range or scrolling.

ProgressBar exposes `track_style`, `fill_style` and `label_style`. Percentage text
uses the normal foreground independently of the colored fill; `show_percentage=false`
leaves only the meter. Committed percentages still update immediately while the
optional fill animation settles. Spinner uses `running_style` and `stopped_style`;
the default stopped appearance is muted. Presentation changes start no additional
timers. All these controls apply disabled styling after role patches.

Tabs exposes `unselected_style`, `indicator_style` and `track_style` alongside
`selected_style`. `show_separator=false` removes the themed indicator rule
and gives its row back to the page. Call `refresh()` when toggling it; page geometry,
measurement and tab mouse hits use the same strip height. Plain themes retain a
one-row strip. Border, padding, captions and help text remain separate components.
