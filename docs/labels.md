# Labels

Use `Label` for display text and `LineEdit` or `TextBox` for editable text.

## Orientation and alignment

`Label` supports horizontal and vertical text. Set its orientation explicitly,
including in a 1×1 view:

```cpp
auto heading = ox::Label{{.text = "Heading", .align = ox::Align::Start}};
auto side = ox::Label{{
    .text = "Side", .vertical = true, .align = ox::Align::End,
}};
```

Horizontal labels draw one line and center it vertically. Vertical labels stack one
complete grapheme per row and center each grapheme horizontally. `Align::Start`,
`Center`, and `End` align the text along its direction: left/center/right horizontally,
top/middle/bottom vertically. `Left` and `Right` remain aliases for Start and End.
The `vertical` flag controls orientation at every allocated size.

## Measurement and fitting

Measurement returns the natural extent before clipping: horizontal terminal columns
by one row, or the widest grapheme by the number of graphemes vertically. Combining
sequences and emoji stay together. Width measurement honors the supplied terminal
width policy. Empty vertical text measures 0×0. Painting keeps a fitting prefix,
never splits wide graphemes, and clears the old label. A vertical grapheme wider than
the view ends the fitting prefix, even if later text would fit. Tabs, line breaks,
controls, unknown widths, and invalid UTF-8 are rejected when measuring/drawing or
calling `set_text()`.

## Size policies

Horizontal defaults are flexible width and height fixed at one. Vertical defaults
reserve at least one column, use natural measured width without consuming spare Row
space, and have flexible height. This lets a wide grapheme reserve two columns by
default. Normal container cross-axis alignment still applies. Explicit width/height
policies override these defaults.

## Change orientation

Use `set_vertical(bool)` to switch at runtime. It updates policies that still match
the preceding orientation's defaults, preserves customized policies, and requests
layout/repaint. Directly editing `.vertical` followed by `refresh()` does the same.
Changing text with `set_text()` also requests layout/repaint.

Button and CheckBox caption options honor `.label.vertical`; those controls retain
their own size policies, so give a vertical caption enough height. Decoration owners
choose direction: Border titles stay horizontal, and Divider labels follow the
Divider's `.vertical` flag. Their embedded label's orientation and size policies
are unused.

For label colors, use [theme defaults and overrides](theme-and-motion.md#colors-and-overrides).
For titles embedded in lines and outlines, see [Borders and dividers](decorations.md).
