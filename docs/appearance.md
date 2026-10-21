# Appearance

Set a theme on the application, then override individual widget styles as needed.

- [Themes](#themes)
- [Colors and overrides](#colors-and-overrides)
- [Editors](#editors)
- [Tables and lists](#tables-and-lists)
- [Indicators](#indicators)
- [Motion](#motion)

## Themes

With your root widget constructed:

```cpp
auto app = ox::Application{root};
auto theme = ox::Theme::dark();
theme.motion.frames_per_second = 60;
app.set_theme(theme);
return app.run({.mouse_mode = ox::MouseMode::Move});
```

`Theme::plain()` is the default and leaves colors to the application.
`Theme::terminal()` uses the terminal's palette and ASCII decorations.
The RGB presets are `dark`, `light`, `ocean`, `ember`, `violet`, `paper`, and
`high_contrast`:

![Dark, Light, Ocean, Ember, Violet, Paper, and High Contrast palette samples](img/theme-palettes.svg)

Set `theme.ascii = true` for ASCII decorations. This does not transliterate text or
pixel graphics. RGB colors depend on terminal support; TermOx does not detect font
or terminal capabilities. See [Theme](https://a-n-t-h-o-n-y.github.io/TermOx/structox_1_1_theme.html) for presets and configuration.

## Colors and overrides

```cpp
auto theme = ox::Theme::ocean();
theme.palette.accent = ox::RGB{0xffcc88};
app.set_theme(theme);
```

The palette provides background, surface, foreground, muted, accent, secondary,
positive, negative, border, and selection roles. Call `set_theme` again after editing
it; theme changes repaint and recalculate layout.

Explicit widget colors override theme defaults. State patches apply afterwards.
A `Style` replaces a complete appearance; a `StylePatch` changes selected parts.
For custom drawing and glyph defaults, see [styles and glyphs](custom-widgets.md#styles-and-glyphs).

Labels can use a semantic role:

```cpp
auto heading = ox::Label{{.text = "Settings", .role = ox::Surface::Accent}};
auto detail = ox::Label{{.text = "Saved locally", .role = ox::Surface::Muted}};
```

Custom widgets can use `appearance(style, Surface::Raised)` or another role, and
`selection_appearance(style, selected_style)` for selection. Raw Canvas drawing
uses the styles you supply.

Call `refresh()` after editing widget presentation fields. Use
`app.run({.mouse_mode = ox::MouseMode::Move})` for hover feedback.

## Editors

```cpp
auto editor = ox::TextBox{{.text = "Notes\n",
                            .line_numbers = true,
                            .highlight_line = true}};
// Compose outer spacing and borders using Padding/Border, and scrollbars using link.
```

TextBox line numbers count logical lines; wrapped continuation rows use a marker.
The gutter does not scroll horizontally and is excluded from `viewport_size()`.
Its width can reflow wrapped text. LineEdit ignores `line_numbers`.

Use `focused_style` and `blurred_style` for focus state, `gutter_style` for line
numbers, and `current_line_style` for the focused display row. Selection remains
visible over the current-line highlight. `inactive_selection_style` applies after
selection styling when unfocused. See [TextBox](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_text_box.html) for the full set of patches.

## Tables and lists

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

Column widths include padding; row heights and separators count toward scroll
extents.

Table cell styling applies in this order: base/focus → column → stripe →
`cell_style` → hover → selection → inactive selection. The `cell_style` callback
returns a patch for visible cells and must not mutate, move, or destroy the table.
Explicit colors override themed colors. See [DataTable](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_data_table.html) for individual options.

```cpp
auto choices = ox::ListView{{.items = {{"Inbox"}, {"Review"}, {"Done"}},
                             .multiple = true,
                             .striped = true,
                             .hover_rows = true,
                             .empty_text = "No matches"}};
```

Lists apply item styles before stripes, hover, and selection; selection wins over
hover. `inactive_selected_style` adjusts an unfocused selection. Set `empty_text`
for an empty list and `show_indicator = false` to remove its marker gutter.
See [ListView](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_list_view.html) for style fields.

Style a Select or ComboBox list through `popup.child`, a TreeView through
`Options::list`, and a LogView or numeric editor through `editor`. TreeView's
`show_guides` and `guide_style` control branch lines independently of selection.

## Indicators

Button's `show_focus_indicator` hides keyboard focus marks without changing focus
styles or spacing. CheckBox, Switch, and RadioGroup style their indicators separately
from labels: use `indicator_style` and the checked/unchecked or selected/unselected
patches. Focus and disabled styles still apply to the control.

Slider, ScrollBar, and ProgressBar expose track and fill/thumb styles. These change
appearance without changing values or scroll positions. Tabs exposes strip styles;
turning off `show_separator` returns that row to the page. Use the
[widget API links](widgets.md) for complete option lists.

## Motion

Animations do not delay input or state changes. Tab contents change immediately
while the underline moves; scroll positions change while the thumb settles.

`theme.motion.enabled = false` makes transitions immediate and stops themed spinner
timers. Application timers and background work continue. There is no automatic
operating-system preference lookup; applications can expose a switch.

Custom widgets can retain an `Animation` member:

```cpp
class Indicator : public ox::Widget {
    ox::Animation amount_{*this};
public:
    void show(double value) {
        amount_.to(value, {theme().motion.feedback, ox::Ease::OutCubic});
    }
    auto paint(ox::Canvas canvas) -> ox::Cursor override {
        ox::paint_meter(canvas, amount_.value(),
                        appearance({}, ox::Surface::Accent),
                        appearance({}, ox::Surface::Border));
        return {};
    }
};
```

It follows widget moves, requests repaint, and unregisters when destroyed. Hidden
subtrees do not schedule frames; disabled widgets settle immediately. Frame callbacks
run on the application thread. Immediate transitions do not invoke them, so model
changes belong in the command that starts the transition.

[Animation](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_animation.html) documents callback and lifetime behavior. Use [Tween](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_tween.html) for
interpolation without a widget or scheduler. The [Explorer](../examples/explorer.md) View menu switches between dark, light, and
terminal themes.
