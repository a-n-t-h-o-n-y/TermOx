# Buttons and choices

Use a Button for an action, a CheckBox for a boolean, and a RadioGroup for one
choice from a list.

```cpp
#include <ox/ox.hpp>

int main()
{
    auto button = ox::Button{{.label = {.text = "Run"}}};
    auto status = ox::Label{"Ready"};
    auto root = ox::Column{std::tie(button, status)};
    ox::Connection{.signal = button.on_activate,
                   .slot = [](ox::Label& label) { label.set_text("Running"); }}(status);
    return ox::Application{root}.run();
}
```

## Button

Button emits `on_activate()` once for an Enter/Space press, or for a captured mouse
press released inside the button. Repeats do not activate, and dragging outside cancels
activation unless the pointer returns before release. `activate()` follows the same
semantic action. `on_press`/`on_release` describe mouse gestures only. Interaction state
is read through `is_pressed()` and `is_hovered()`.

## CheckBox

```cpp
auto notifications = ox::CheckBox{{.checked = true,
                                    .label = {.text = "Notifications"}}};
notifications.set_checked(false);
```

Read `checked()` and use `set_checked(bool)` or `toggle()` to change it.
`on_change(bool)` emits once per actual change. Clicking the caption also toggles
the control; Enter/Space repeats do not toggle it.

## RadioGroup

```cpp
auto quality = ox::RadioGroup{{.items = {"Draft", "Normal", "High"},
                                .selected_index = 1}};
quality.select(2);
```

Read `selected_index()`; `select(index)` changes it and `select(std::nullopt)` clears
it. Invalid indices throw. Replacing items with `set_items` clears selection.
`on_select` emits only on a change and supplies an index snapshot, so duplicate
labels remain distinct and callbacks can safely replace items.

Arrows move and select; Enter/Space or a left click selects the highlighted row.
Home/End/PageUp/PageDown and the wheel navigate longer lists. Disabled rows are
skipped. RadioGroup supports [vertical scrolling](scrolling.md).

## Layout and repaint

Commands request repaint and any needed layout. After editing public configuration,
call `refresh()`. See [presentation](presentation.md#buttons-and-choice-indicators)
for indicator and focus styles, and [Connections](connections.md) for callback
ownership. Custom controls can reuse the [activation and selection helpers](internals/controls.md).
