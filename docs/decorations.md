# Borders and dividers

Wrap a widget with `Border` for an outline and title. Put `Divider` between
sections of a layout. Both keep their labels as plain `Label::Options` values,
without a separate widget lifetime, focus state, or child enumeration. Border
configuration is copyable.

```cpp
#include <ox/ox.hpp>
using namespace ox;

int main()
{
    auto panel = Column{
        Label{"First"},
        Divider{{.label = {.text = "Section"}}},
        Label{"Second"}} | Border{.label = {.text = "Panel"}};
    return Application{panel}.run();
}
```

## On this page

- [Borders](#borders)
- [Dividers](#dividers)
- [Paint decoration text](#paint-decoration-text)
- [Related guides](#related-guides)

## Borders

`border_content_rect(Area)` gives the child's parent-local rectangle. The border uses
one cell on each side; dimensions smaller than two leave an empty interior. Its origin
is clamped to the available area, including zero-sized views. Bordered uses this function
for construction and arrangement. `paint_border(Canvas, Border const&)` draws the box
and overlays the title along the top edge between the corners, leaving the interior
untouched. Label alignment/style are independent of the box style. Label width/height policies
are unused in decorations. Bordered still exposes child/border and supports `widget | Border`.

## Dividers

Divider has explicit `vertical` configuration, defaulting to false, even in a 1×1 view.
Put a vertical divider in a Row and a horizontal one in a Column. Its width is fixed(1) when vertical; its height is fixed(1) when horizontal. Larger views draw the line down the middle of
the cross axis. `paint_divider` clears its view, draws that line, and overlays its label.
Horizontal labels are ordinary text; vertical labels stack one grapheme per row, with
each centered across the width. For vertical labels, `Align::Start/Center/End` means
top/middle/bottom along the vertical axis. A glyph too wide for the view ends the fitting
prefix. Lines::ascii uses `|` vertically and `-` horizontally.

## Paint decoration text

`put_label(Canvas, text, align, style, vertical=false)` draws text without clearing
the surrounding line. `paint_label` takes the same optional orientation and clears
the view to styled spaces first. Both validate the complete UTF-8 input and obey
the canvas width policy and clip. Orientation, natural measurement, accepted text,
and fitting-prefix rules have their primary explanation in [Labels](labels.md).
Border titles stay horizontal; Divider labels follow the divider's orientation.
Embedded label orientation and size policies are not used by these owners.

## Related guides

- [Layout and containers](containers.md): ownership, spacing, and changing layouts.
- [Widget presentation](presentation.md): focus, selection, and control indicators.
- [Custom drawing](drawing.md): paint functions and clipped canvases.
