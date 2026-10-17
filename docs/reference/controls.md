# Controls and data views

[API reference](../reference.md) · [Guides](../README.md)

Buttons, choices, tables, scrolling, and composite widgets.

## On this page

- [ox::Button](#oxbutton)
- [ox::CheckBox](#oxcheckbox)
- [ox::RadioGroup](#oxradiogroup)
- [ox::DataTable](#oxdatatable)
- [ox::ScrollBar](#oxscrollbar)
- [Selection and application widgets](#selection-and-application-widgets)

## ox::Button

[`#include <ox/button.hpp>`](../../include/ox/button.hpp)

`class Button : public Widget;`

Button composes a label, style patches, and optional animation. Connect actions to
`on_activate`; Enter/Space presses activate once, and a captured mouse gesture activates
on release inside. Repeats do not activate. `on_press`/`on_release` describe mouse gestures.
Read state through `is_pressed()` and `is_hovered()`.

```cpp
auto button = Button{{.label = {.text = "Run"}}};
button.on_activate.connect([] { /* Run the action. */ });
```

## ox::CheckBox

`<ox/checkbox.hpp>` supplies a checkbox with a plain label caption.
Read `checked()`; mutate with `set_checked(bool)` or `toggle()`. `on_change(bool)` emits
only for changes. Glyph vectors and focused style remain public configuration.

## ox::RadioGroup

`<ox/radiogroup.hpp>` supplies a vertical group with owned item strings. Read `items()`,
`selected_index()`, and `highlighted_index()`; mutate with `set_items` and `select`.
`select(std::nullopt)` clears selection; invalid indices throw. Arrows move and select,
while Enter/Space or click selects. `on_select(optional<size_t>)` emits only for changes.
See [control composition](../controls.md) for activation and state contracts.

## ox::DataTable

A rectangular table with owned row vectors, column descriptions, fixed headers,
Unicode-safe ellipsis, two-axis scrolling, keyboard/mouse row selection, activation,
and stable sorting. Columns compose SizePolicy, Align, and StylePatch. Data access is
through const spans; mutation functions validate shape and UTF-8 before committing.

See [data tables and scrolling](../data-tables.md) for the complete API, examples,
callbacks, sorting/filtering, and viewport rules. The interactive example
is [examples/datatable.cpp](../../examples/datatable.cpp).

## ox::ScrollBar

A vertical or horizontal scroll control with content_length, viewport_length,
position, style, and optional scroll_settle_time animation. set_position notifies
only on change; set_range synchronizes without feedback. Shared pure functions in
`<ox/scroll.hpp>` compute limits, movement, reveal ranges, and thumb/click geometry.

See [data tables and scrolling](../data-tables.md#shared-scrolling-functions).

## Selection and application widgets

`ListView`, `Select`, `ComboBox`, `Tabs`, `Popup`, `Dialog`, `Menu`, and `MenuBar` compose
shared row selection, clipping, text editing, containers, and routing. See the
[selection and application guide](../selection-and-application.md) for commands, signal
contracts, keyboard/mouse bindings, lifetime rules, and examples.

See [values and composite widgets](../value-and-composite-widgets.md) for Slider,
NumericEdit, ProgressBar, Spinner, Switch, TreeView, LogView, SplitPane, Form,
FilePicker, and CommandPalette.

### Typed numeric input and shared scrolling

`NumericEdit` is `BasicNumericEdit<double>`; `IntegerEdit` is
`BasicNumericEdit<int>`. Bounds, steps, values, and `on_change` signals use the numeric
type. Configure `.format`, `.prefix`, `.suffix`, `.locale`, and
`.commit_on_focus_loss` without changing the simple default construction. See
[values and composite widgets](../value-and-composite-widgets.md) for precision-safe
editing and form commit behavior.

All standard scrollable views implement `Scrollable` and use the same
`link(view, scrollbar)` function. RadioGroup takes `Point{0, row}` offsets;
TreeView and LogView expose scrolling directly. See
[shared scrolling](../containers.md#shared-scrolling-interface).

See [shared commands](../commands.md) for commands reused by menus, palettes, shortcuts, and KeyHelp.

See [editor and table presentation](../presentation.md) for optional TextBox gutters and
current-line styling, DataTable density/rules/hover, style precedence and sort markers.
