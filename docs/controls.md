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

`activates` accepts an unmodified Enter/Space press or repeat, or a left mouse press.
Releases never activate. `releases` accepts an activation-key release even if modifiers
have changed, or a left/unspecified mouse release. Widget policy determines whether a
release matches its current pressed state; these functions do not capture input.

`select_index(selected, index, count)` rejects out-of-range values, returning no effects.
A valid value is handled and requests repaint only if it differs. `move_selection`
clamps navigation, initializes an empty optional to zero, and safely normalizes stale
indices after item removal. Empty lists clear the optional and remain unhandled.
Both return EventResponse and never schedule painting or emit callbacks.

Button derives directly from Widget. It owns public label/decoration configuration,
pressed/hover state, and fade percentage. Its focused and pressed appearances are
StylePatch values; it paints through `paint_label(Canvas, text, align, style)` without
creating a Label widget. Focus comes from Widget. Enter/Space/left-click emits `on_press`;
a paired release emits `on_release`. Leaving or losing focus cancels its pressed
appearance. Fade target is hover OR focus, so leaving one state does not cancel the
other. A lazy, move-aware Timer drives its existing 10 ms fade steps; it stops at the
target or when decoration is no longer Fade. Nonpositive durations jump to the target.

CheckBox owns a bool and glyph vectors. `set_checked(value)`/`toggle()` notify only on
change, using one `on_toggle(bool)` signal. RadioGroup owns item strings and optional
selected/highlighted indices; navigation and activation are separate. `select(value)`
rejects invalid indices and emits a stable string snapshot only when selection changes.
Commands request repaint when attached. Direct field edits emit no signal and need an
explicit application repaint request, just like other widget configuration changes.

Glyph-span `put` accepts a StylePatch; styled UTF-8/UTF-32 `put` accepts a Style. This
avoids copying glyph vectors merely to apply focus styling. Radio indicators are
measured in terminal columns, so a multi-scalar emoji does not misalign its label.

## API migration

| Previous API | Current API |
| --- | --- |
| BasicButton | Button with an empty/default label |
| Button::press_mod / focus_mod | pressed_style / focused_style StylePatch |
| Private button decoration | Public Decoration variant and fade_percent |
| CheckBox::State and Display | bool checked, checked_glyphs, unchecked_glyphs, focused_style |
| check/uncheck/set_state/get_state | set_checked(true/false), set_checked(value), checked |
| on_check/on_uncheck | on_toggle(bool) |
| RadioGroup::in_focus_style | focused_style StylePatch |
| Private radio focus selection | Public optional highlighted index |
| Repeated on_select for unchanged activation | Notifications only when selection changes |
| RadioGroup default focus None | Strong |

The previous classes/types/callback adapters are removed. There is no compatibility
layer. Input effects still flow through the application boundary, and explicit widget
commands can request repaint directly.
