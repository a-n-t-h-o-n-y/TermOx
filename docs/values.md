# Numeric input and status

Use `Slider` to choose a value visually, `NumericEdit` for decimal input, and
`IntegerEdit` for whole numbers.

```cpp
#include <ox/ox.hpp>

int main()
{
    auto slider = ox::Slider{{.range = {0, 1, .05}, .value = .5}};
    auto number = ox::NumericEdit{{.range = {0, 1, .05}, .value = .5}};
    auto root = ox::Column{std::tie(slider, number)};
    ox::Connection{.signal = slider.on_change,
                   .slot = [](double value, ox::NumericEdit& n) {
                       n.set_value(value);
                   }}(number);
    return ox::Application{root}.run();
}
```

## Ranges and stepping

Ranges use inclusive bounds and a positive step. Programmatic values clamp to the
range without snapping to a step; invalid ranges and non-finite values throw.
Read `value()` and change it with `set_value`; `on_change` emits only on a change.
`IntegerEdit` stores and emits `int`, rejecting fractions and overflow.

Slider arrows move one step: Left/Down decrease and Right/Up increase. Home/End
select the bounds; PageUp/PageDown move ten steps. Vertical sliders put the maximum
at the top. Dragging continues outside the track and clamps to the range.

## Commit numeric drafts

```cpp
auto count = ox::IntegerEdit{{.range = {0, 1000, 1}, .value = 3}};
auto weight = ox::NumericEdit{{
    .range = {0, 100, .5}, .value = 12.345,
    .format = "{:.2f}", .suffix = " kg",
    .commit_on_focus_loss = true,
}};
```

Editing creates a draft. Enter or `commit()` accepts a number in range; invalid
text remains visible, emits `on_invalid`, and leaves the value unchanged.
`commit()` reports validity even when the value is unchanged. Use `has_draft()`
to distinguish staged input from display text. Up/Down steps from the committed
value; Escape or `revert()` discards the draft. `set_value` replaces any draft.

Focus loss preserves drafts by default. With `commit_on_focus_loss = true`, it
attempts a commit; invalid text still stays visible. [Forms](forms.md) commit
numeric drafts before validating fields and submitting application data.

## Format values

The format applies to unfocused committed values. Focusing an unchanged field
exposes full precision, so `"{:.2f}"` cannot silently round the stored value.
Drafts remain as typed until commit or revert. Prefixes and suffixes are
noneditable decorations; numeric labels support them too.

The editor's locale controls parsing and editing text. Use `L` in the format,
such as `"{:.2Lf}"`, for locale-aware display. Formats must produce printable
single-line text; invalid formats throw. Use the public `editor` for selection,
clipboard, and [text styling](presentation.md#textbox-and-lineedit). Call
`refresh()` after changing presentation fields.

## Show progress and activity

`ProgressBar` displays a value without accepting focus. Use `show_percentage` to
toggle its centered label. `Spinner` displays animated UTF-8 frames; control it
with `set_running`. Stopped spinners have no active timer, and hidden, disabled,
or out-of-scope widgets follow the application's timer eligibility rules.

`Switch` uses the same boolean state and signals as [CheckBox](controls.md#checkbox)
with a different indicator. For colors and animation, see
[presentation](presentation.md#values-activity-and-tabs) and [motion](theme-and-motion.md#motion).
