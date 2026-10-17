# Control composition

`<ox/controls.hpp>` supplies activation and index-selection functions without widgets,
item ownership, signals, or event-loop state. Button, CheckBox, and RadioGroup use the
same functions; custom controls can compose them with ordinary C++ state.

```cpp
#include <ox/controls.hpp>

auto selected = std::optional<std::size_t>{};
auto response = ox::move_selection(selected, 5, 1); // Highlight the first item.
response = ox::select_index(selected, 3, 5);        // Select index three.
auto key = ox::KeyEvent{.key = U' '};
if (ox::activates(key)) { /* Activate the highlighted item. */ }
```

## On this page

- [Activation and selection helpers](#activation-and-selection-helpers)
- [Button](#button)
- [CheckBox](#checkbox)
- [RadioGroup](#radiogroup)
- [Layout and repaint](#layout-and-repaint)

## Activation and selection helpers

`activates` accepts an unmodified Enter/Space press or a left mouse press.
Key repeats and releases do not activate. `releases` recognizes activation-key
releases and left/unspecified mouse releases; widgets match these to their own gestures.

`select_index(selected, index, count)` rejects out-of-range values, returning no effects.
A valid value is handled and requests repaint only if it differs. `move_selection`
clamps navigation, initializes an empty optional to zero, and safely normalizes stale
indices after item removal. Empty lists clear the optional and remain unhandled.
Both return EventResponse and never schedule painting or emit callbacks.

## Button

Button emits `on_activate()` once for an Enter/Space press, or for a captured mouse
press released inside the button. Repeats do not activate, and dragging outside cancels
activation unless the pointer returns before release. `activate()` follows the same
semantic action. `on_press`/`on_release` describe mouse gestures only. Interaction state
is read through `is_pressed()` and `is_hovered()`; label, style patches, decoration, and
fade configuration remain ordinary public values. Fade follows hover OR focus.

## CheckBox

CheckBox owns its checked state. Read `checked()` and use `set_checked(bool)` or
`toggle()`; `on_change(bool)` emits once per actual change. Its plain `Label::Options label` adds a clickable caption beside the indicator. Enter/Space repeats do not toggle.

## RadioGroup

RadioGroup owns its items and optional selected/highlighted indices. `items()` returns
a const span; `set_items(...)` replaces the items and clears selection. `select(index)`
selects, `select(std::nullopt)` clears, and invalid indices throw. `selected_index()` and
`highlighted_index()` expose state. Arrows move and select; Enter/Space or a left click
selects the highlighted row. `on_select(std::optional<std::size_t>)` emits only when
selection changes, preserving identity even when labels repeat. Slots may replace items;
notification values are snapshots.

## Layout and repaint

Controls use `appearance(style)` for disabled styling. Commands request repaint, and
changes affecting measurement request layout. For public configuration edits, call
`refresh()` to recalculate layout and repaint. Named
`Connection{.signal = signal, .slot = callback}(widgets...)` remains supported.
